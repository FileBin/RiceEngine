cmake_minimum_required(VERSION 3.19)

project("compcompiler")

set(RICE_GL_NAME ${PROJECT_NAME} CACHE INTERNAL "")
set(RICE_GL_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 20)

#file search
file(GLOB TESTS "${CMAKE_CURRENT_SOURCE_DIR}/Test*")

foreach(TEST ${TESTS})
    add_subdirectory(${TEST})
endforeach(TEST ${TESTS})
