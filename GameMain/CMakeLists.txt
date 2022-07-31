cmake_minimum_required(VERSION 3.19)
project("GameMain")

add_subdirectory("res")

set(CMAKE_CXX_STANDARD 20)

#file search
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_executable (${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h)

include_directories(${PROJECT_NAME} ${RICE_INCLUDE_PATH})
include_directories(${PROJECT_NAME} ${LIB_INCLUDE_PATH})
include_directories(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_directories(${PROJECT_NAME} PUBLIC
        ${ANY_LIB_BIN_PATH}
        ${LIB_BIN_PATH}
        ${PROJECT_BINARY_DIR}
        ${DXSDK_LIB_PATH}/x64
        ${VULKAN_LIB_PATH}
        )

set(VLK_NAME "vulkan")

if(WIN32)
    set(VLK_NAME "vulkan-1")
endif(WIN32)

set(OTHER_LIBS "")

if(UNIX)
    set(OTHER_LIBS "dl")
endif(UNIX)


target_link_libraries(${PROJECT_NAME}
    ${RICE_LIBS}
    #"OpenAL32"
    "fmt"
    "ogg"
    "vorbis"
    "vorbisfile"
    "SDL2"
    "SDL2main"
    ${VLK_NAME}
    "vk-bootstrap"
	${OTHER_LIBS}
)

add_dependencies(${PROJECT_NAME} ${RICE_NAME} ${GAME_RESOURCES_NAME})

if(WIN32)
    #copy dll
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD   # Adds a post-build event to the target
        COMMAND ${CMAKE_COMMAND}
            -E copy_if_different                    # which executes "cmake - E copy_if_different..."
            ${RICE_DLLs_x64}                        # <--this is in-file
            ${CMAKE_BINARY_DIR})                    # <--this is out-file path
endif(WIN32)