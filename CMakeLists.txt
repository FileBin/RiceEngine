cmake_minimum_required(VERSION 3.24)

project(RiceEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

option(RICE_BUILD_TESTS "Build the test/demo executables" ON)
# The 2021-era headers assume a common prelude in every translation unit
# (typedefs from Rice/Util/defines.hpp, namespaces, fmt, SDL) and do not include
# their own dependencies. Forcing the module's pch.h into every TU restores that
# build order without depending on a PCH.
function(rice_force_include target header)
    if(MSVC)
        target_compile_options(${target} PRIVATE "/FI${header}")
    else()
        target_compile_options(${target} PRIVATE -include "${header}")
    endif()
endfunction()
option(RICE_BUILD_COMPONENT_BUILDER "Build the component code generator" OFF)

# Headers of RiceMetaCompiler (pinned through the flake, see flake.nix).
set(RICE_METACOMPILER_INCLUDE_DIR "" CACHE PATH "Include directory of RiceMetaCompiler")

add_subdirectory(RiceEngine)
