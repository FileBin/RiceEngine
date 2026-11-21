# Rice Engine

## About

Rice Engine is built for making high-performance games for Windows and Linux (and Android probably).

This Engine uses object and components to build your game scene (like in Unity). It also has client-server arhitechure. You can also make plugins for Rice Engine.

## Current features

* Render engine is alomost done, but needs improvement.
* Scene object system is done.
* Math and Utlis also working.
* Tested compability with windows.

## 🚧 In development

* Plugin system for engine
* Network Library
* Reflection System
* Serialization (almost done in dev branch)
* Dynamic component loading system (from .so or .dll)
* Audio System.

## Building

The build is reproducible through a Nix flake: every third-party dependency
(SDL2, fmt, Vulkan headers/loader, vk-bootstrap, magic_enum, nlohmann_json,
libogg/libvorbis, libcds, RiceMetaCompiler) comes from the flake and is pinned in
`flake.lock`. Nothing third-party is vendored in the repository.

```bash
# dev shell: cmake, ninja, gcc, glslc, vulkan-tools, all dependencies
nix develop

# the same, plus VS Code with clangd / C++ / CMake / GLSL extensions
nix develop .#vscode
./develop.sh            # opens VS Code in this directory inside that shell

# inside the shell
cmake -S . -B build -G Ninja
cmake --build build
# samples land in build/RiceEngine/Tests (CubeTest, QuadTest, MultiCubePerformanceTest, ...)

# or build the whole thing hermetically and use the result
nix build .#default
ls result/bin
```

`shell.nix` is kept for `nix-shell` and editor integrations: it is a thin alias
for `nix develop .#vscode`, so both entry points give the same environment and
the VS Code extension list that used to live in that file now lives in
`flake.nix`.

## Tests

`nix flake check` builds everything and runs the demos headlessly:

```bash
nix flake check                 # everything
nix build .#checks.x86_64-linux.smoke-cube   # one check, with -L for the log
```

* `build` - the package itself (release build).
* `cpu-concurrent-queue`, `cpu-concurrent-list` - the lock-free container tests,
  no display needed.
* `smoke-cube`, `smoke-multi-cube`, `smoke-quad` - the demos run against sway's
  headless backend and Mesa's lavapipe (CPU Vulkan) in a Debug build, which is
  what turns the Vulkan validation layers on. Each check asserts that the
  swapchain is created at the drawable size reported by the compositor, that
  resizing the window recreates it, that no validation error is reported, and
  that closing the window makes the engine exit with code 0.

The checks run inside the Nix sandbox: they never touch the host GPU and never
open a window on the desktop.

libcds and RiceMetaCompiler are flake inputs (pinned by `flake.lock`) instead of
git submodules, and are built from their upstream sources on demand:

```bash
nix build .#libcds .#rice-meta-compiler
```

## Documentation

[Link](doc/documentation.md)
