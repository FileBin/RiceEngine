cmake_minimum_required(VERSION 3.19)

project("RiceEngine")

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory("lib")

set(RICE_NAME ${PROJECT_NAME} CACHE INTERNAL "")
set(RICE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include; ${LIB_INCLUDE_PATH} CACHE INTERNAL "")

add_subdirectory("Math")
add_subdirectory("Util")
add_subdirectory("GL")
add_subdirectory("Networking")

add_subdirectory("Rice")

add_subdirectory("Tests")
add_subdirectory("ComponentBuilder")
add_subdirectory("MetaCompiler")