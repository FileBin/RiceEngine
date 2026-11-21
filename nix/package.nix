{ lib
, stdenv
, cmake
, ninja
, pkg-config
, shaderc
, vulkan-headers
, vulkan-loader
, vk-bootstrap
, SDL2
, fmt
, magic-enum
, nlohmann_json
, libogg
, libvorbis
, libcds
, rice-meta-compiler
, enableTests ? true
, enableComponentBuilder ? false
# Debug build defines !NDEBUG, which is what turns on the Vulkan validation
# layers in GL/src/config.h (GL_DEBUG_MODE). Used by the flake checks.
, debugBuild ? false
}:

stdenv.mkDerivation {
  pname = "rice-engine";
  version = "0.0.2";

  # The flake only sees git-tracked files, so this is already the "sources" tree.
  src = lib.cleanSourceWith {
    src = ../.;
    filter = path: type:
      let base = baseNameOf path; in
      !(builtins.elem base [ ".git" "build" "result" ]);
  };

  nativeBuildInputs = [ cmake ninja pkg-config shaderc ];
  buildInputs = [
    vulkan-headers
    vulkan-loader
    vk-bootstrap
    SDL2
    fmt
    magic-enum
    nlohmann_json
    libogg
    libvorbis
    libcds
  ];

  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=${if debugBuild then "Debug" else "RelWithDebInfo"}"
    "-DRICE_BUILD_TESTS=${if enableTests then "ON" else "OFF"}"
    "-DRICE_BUILD_COMPONENT_BUILDER=${if enableComponentBuilder then "ON" else "OFF"}"
    "-DRICE_METACOMPILER_INCLUDE_DIR=${rice-meta-compiler}/include"
  ];

  meta = {
    description = "Rice Engine — game engine for high performance and network based games";
    platforms = lib.platforms.linux;
    mainProgram = "CubeTest";
  };
}
