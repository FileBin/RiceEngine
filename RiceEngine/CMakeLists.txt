# ---------------------------------------------------------------------------
# Third-party dependencies.
#
# Nothing third-party is vendored in this repository: headers and binaries used
# to live in RiceEngine/lib and RiceEngine/ExternalLibs. They are now taken from
# the build environment, which the flake pins (flake.nix + flake.lock).
# ---------------------------------------------------------------------------

find_package(Vulkan REQUIRED)                # vulkan-headers + vulkan-loader
find_package(SDL2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(vk-bootstrap CONFIG REQUIRED)
find_package(LibCDS CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(OGG REQUIRED IMPORTED_TARGET ogg)
pkg_check_modules(VORBIS REQUIRED IMPORTED_TARGET vorbis vorbisfile)

# One interface target carries every external dependency, so the modules below
# only ever link "rice_deps".
add_library(rice_deps INTERFACE)
target_link_libraries(rice_deps INTERFACE
    Vulkan::Vulkan
    SDL2::SDL2
    fmt::fmt
    magic_enum::magic_enum
    nlohmann_json::nlohmann_json
    vk-bootstrap::vk-bootstrap
    LibCDS::cds-s
    PkgConfig::OGG
    PkgConfig::VORBIS
)

# Every module includes the engine's shared headers (Rice/config.h,
# Rice/namespaces.h, Rice/stdafx.hpp) from Rice/include. Interface target so the
# include dir is declared once and exported to anything that links it.
add_library(rice_headers INTERFACE)
target_include_directories(rice_headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Rice/include")

add_subdirectory("Math")
add_subdirectory("Util")
add_subdirectory("GL")
add_subdirectory("Networking")
add_subdirectory("Rice")

if(RICE_BUILD_TESTS)
    add_subdirectory("Tests")
endif()

if(RICE_BUILD_COMPONENT_BUILDER)
    add_subdirectory("ComponentBuilder")
endif()
