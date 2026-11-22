# Kept for `nix-shell` (and tools that expect a shell.nix, e.g. direnv or the
# VS Code "Nix Environment Selector"). The environment itself lives in
# flake.nix now, so both entry points give the same thing:
#
#   nix-shell  ==  nix develop .#vscode
#
# That is the build toolchain (cmake, ninja, gcc, glslc, Vulkan, SDL2, fmt,
# vk-bootstrap, libcds, ...) plus VS Code with the extension set this file used
# to declare.
let
  flake = builtins.getFlake (toString ./.);
in
  flake.devShells.${builtins.currentSystem}.vscode
