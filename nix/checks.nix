# Checks for `nix flake check`.
#
# Rice Engine is a graphics application, so a meaningful smoke test has to open
# a window, create a Vulkan surface and build a swapchain. These checks run the
# demo binaries against sway's headless backend and Mesa's lavapipe (CPU
# Vulkan): everything works inside the Nix sandbox, never touches the host GPU
# and never opens a window on the user's desktop.
#
# The demos are built in Debug mode on purpose - that is what enables the Vulkan
# validation layers (GL_DEBUG_MODE in GL/src/config.h), so any API misuse shows
# up as an [ERROR] line in the log.
#
# Asserted per demo:
#   1. the swapchain is created at the drawable size reported by the compositor
#      (regression: the image used to be smaller than the window);
#   2. resizing the window recreates the swapchain (regression: the render scale
#      did not follow the window);
#   3. the run produces no Vulkan validation errors;
#   4. closing the window makes the engine shut down with exit code 0.
{ pkgs ? import <nixpkgs> { } }:

let
  inherit (pkgs) lib;

  # Validation layers are only requested in a Debug build.
  engine = pkgs.rice-engine.override { debugBuild = true; };

  # Everything the compositor and the loader need, exported by the script
  # itself: putting WLR_* into derivation env attributes made sway pick the DRM
  # backend inside the sandbox and hang until libseat timed out.
  compositorEnv = ''
    export WLR_BACKENDS=headless
    export WLR_LIBINPUT_NO_DEVICES=1
    export WLR_RENDERER=pixman
    export SDL_VIDEODRIVER=wayland
    export VK_DRIVER_FILES=${pkgs.mesa}/share/vulkan/icd.d/lvp_icd.x86_64.json
    export VK_LAYER_PATH=${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d
  '';

  smoke = name: pkgs.runCommandLocal "rice-check-${name}" {
    nativeBuildInputs = with pkgs; [
      # sway-unwrapped, not sway: the wrapper starts dbus-run-session, which
      # needs /etc/dbus-1/session.conf that does not exist in the sandbox, and
      # sway does not need a session bus for anything these checks do.
      sway-unwrapped
      mesa
      vulkan-validation-layers
      gnugrep
      gawk
      coreutils
    ];
  } ''
    set +e   # the generated builder runs with -e; we handle failures ourselves
    set -u
    export HOME="$TMPDIR/home"
    export XDG_RUNTIME_DIR="$TMPDIR/xdg"
    mkdir -p "$HOME" "$XDG_RUNTIME_DIR"
    chmod 700 "$XDG_RUNTIME_DIR"
    ${compositorEnv}

    # On failure print everything that helps debugging; on success stay quiet.
    fail() {
      echo "FAIL: $*"
      echo "--- app log ---"
      cat "''${APP_LOG:-/nonexistent}" 2>/dev/null || true
      echo "--- compositor log (tail) ---"
      tail -15 "$HOME/sway.log" 2>/dev/null || true
      exit 1
    }

    sway -d -c /dev/null > "$HOME/sway.log" 2>&1 &
    SWAY_PID=$!
    trap 'kill $SWAY_PID 2>/dev/null || true' EXIT

    # Give the compositor a moment, then look for its socket (test -S, no pipes).
    sleep 4
    kill -0 $SWAY_PID 2>/dev/null || { echo "--- sway log ---"; cat "$HOME/sway.log"; fail "the headless compositor exited"; }

    SOCK=""
    for _ in $(seq 1 100); do
      for s in "$XDG_RUNTIME_DIR"/wayland-*; do
        [ -S "$s" ] && SOCK="$s" && break 2
      done
      sleep 0.1
    done
    [ -n "$SOCK" ] || { echo "--- sway log ---"; cat "$HOME/sway.log"; fail "no wayland socket"; }
    export WAYLAND_DISPLAY=$(basename "$SOCK")

    SWAYSOCK=""
    for s in "$XDG_RUNTIME_DIR"/sway-ipc.*; do
      [ -S "$s" ] && SWAYSOCK="$s" && break
    done
    [ -n "$SWAYSOCK" ] || fail "no sway IPC socket"
    export SWAYSOCK

    # The demos load shaders relative to the working directory, so run a copy of
    # bin/ (it also makes the log file writable).
    mkdir -p "$TMPDIR/work"
    cp -r ${engine}/bin/. "$TMPDIR/work"/
    cd "$TMPDIR/work"

    APP_LOG="$TMPDIR/work/app.log"
    ./${name} > "$APP_LOG" 2>&1 &
    APP_PID=$!
    sleep 6
    kill -0 $APP_PID 2>/dev/null || {
      echo "--- app log ---"; cat app.log
      fail "${name} died during startup"
    }
    BUILDS_BEFORE=$(grep -c "Swapchain built" app.log || true)

    # A resize has to recreate the swapchain; make the window floating first so
    # the request is not overruled by tiling.
    swaymsg -t command "[app_id=\"${name}\"] floating enable" >/dev/null 2>&1 || true
    swaymsg -t command "[app_id=\"${name}\"] resize set 800 600" >/dev/null 2>&1 || true
    sleep 3
    BUILDS_AFTER=$(grep -c "Swapchain built" app.log || true)

    swaymsg -t command "[app_id=\"${name}\"] kill" >/dev/null 2>&1 || true
    for _ in $(seq 1 150); do
      kill -0 $APP_PID 2>/dev/null || break
      sleep 0.1
    done
    if kill -0 $APP_PID 2>/dev/null; then
      kill -9 $APP_PID 2>/dev/null || true
      cat app.log
      fail "${name} did not exit after the window was closed"
    fi
    wait $APP_PID
    STATUS=$?

    echo "${name}: swapchain builds = ''${BUILDS_AFTER}, exit = ''${STATUS}"
    grep "Swapchain built" app.log | sed 's/^/    /'

    [ "$STATUS" -eq 0 ] || fail "${name} exited with $STATUS"
    [ "$BUILDS_BEFORE" -ge 1 ] || fail "the swapchain was never built"
    [ "$BUILDS_AFTER" -gt "$BUILDS_BEFORE" ] \
      || fail "resizing the window did not recreate the swapchain"

    # Swapchain extent must equal the drawable extent in every build.
    MISMATCH=$(grep "Swapchain built" app.log \
      | sed -n 's/.*built! \([0-9]*\)x\([0-9]*\), .*drawable \([0-9]*\)x\([0-9]*\)).*/\1 \2 \3 \4/p' \
      | awk '$1 != $3 || $2 != $4')
    [ -z "$MISMATCH" ] || fail "swapchain size differs from the drawable size: $MISMATCH"

    ERRORS=$(grep "\[ERROR\]" app.log || true)
    [ -z "$ERRORS" ] || fail "the engine reported errors: $ERRORS"

    mkdir -p $out
  '';

  # The lock-free containers need no display at all; they exit on their own.
  cpuTest = name: pkgs.runCommandLocal "rice-check-${name}" {
    nativeBuildInputs = [ pkgs.coreutils ];
  } ''
    set -u
    cd "$TMPDIR"
    ${engine}/bin/${name} > out.log 2>&1
    STATUS=$?
    cat out.log
    [ "$STATUS" -eq 0 ] || { echo "FAIL: ${name} exited with $STATUS"; exit 1; }
    mkdir -p $out
  '';
in
{
  # Building the release package is itself a check.
  build = engine;

  cpu-concurrent-queue = cpuTest "ConcurrentQueueTests";
  cpu-concurrent-list = cpuTest "ConcurrentListTest";
  # Util::RegisterCollection free-list regression (no display, no Vulkan).
  cpu-register-collection = cpuTest "ObjectChurnTest";

  smoke-cube = smoke "CubeTest";
  smoke-multi-cube = smoke "MultiCubePerformanceTest";
  smoke-quad = smoke "QuadTest";
}
