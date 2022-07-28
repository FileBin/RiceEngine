cmake_minimum_required(VERSION 3.19)

set(RESOURCES_PATH "${CMAKE_SOURCE_DIR}/res")

file(GLOB_RECURSE RESOURCES "${RESOURCES_PATH}/*.*")

#copy resources
add_custom_target(Resources ALL DEPENDS ${RESOURCES})

set(OUT_RES_DIR ${CMAKE_BINARY_DIR}/resources)

message(${OUT_RES_DIR})

file(MAKE_DIRECTORY ${OUT_RES_DIR})

add_custom_command(TARGET Resources
    COMMAND ${CMAKE_COMMAND}
        -E copy_if_different             # which executes "cmake - E copy_if_different..."
        ${RESOURCES_PATH}             # <--this is in-file
        ${OUT_RES_DIR})          	  # <--this is out-file path