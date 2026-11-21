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

# inside the shell
cmake -S . -B build -G Ninja
cmake --build build
# samples land in build/RiceEngine/Tests (CubeTest, QuadTest, MultiCubePerformanceTest, ...)

# or build the whole thing hermetically and use the result
nix build .#default
ls result/bin
```

libcds and RiceMetaCompiler are flake inputs (pinned by `flake.lock`) instead of
git submodules, and are built from their upstream sources on demand:

```bash
nix build .#libcds .#rice-meta-compiler
```

## Documentation

[Link](doc/documentation.md)
