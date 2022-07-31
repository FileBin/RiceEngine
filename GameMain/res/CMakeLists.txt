cmake_minimum_required(VERSION 3.19)

set(RESOURCES_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE RESOURCES "${RESOURCES_PATH}/*.*")

set(GAME_RESOURCES_NAME GameResources CACHE INTERNAL "")

#copy resources
add_custom_target(${GAME_RESOURCES_NAME} ALL DEPENDS ${RESOURCES})

set(OUT_RES_DIR ${CMAKE_BINARY_DIR}/resources)

message(${OUT_RES_DIR})

file(MAKE_DIRECTORY ${OUT_RES_DIR})

add_custom_command(TARGET ${GAME_RESOURCES_NAME}
    COMMAND ${CMAKE_COMMAND}
        -E copy_directory
        ${RESOURCES_PATH}
        ${OUT_RES_DIR})