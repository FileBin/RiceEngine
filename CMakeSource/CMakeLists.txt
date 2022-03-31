cmake_minimum_required (VERSION 3.8)

#project config
project ("GameEngine")
project ("GameMain")

set(GAME_ENGINE_PATH "${CMAKE_SOURCE_DIR}/src/RiceEngine")
set(GAME_MAIN_PATH "${CMAKE_SOURCE_DIR}/src/GameMain")
set(LIB_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/lib/include")

set(VULKAN_LIB_PATH "$ENV{VK_SDK_PATH}/Lib")
set(VULKAN_INCLUDE_PATH "$ENV{VK_SDK_PATH}/Include")

set(APPEND DXSDK_LIB_PATH "$ENV{DXSDK_DIR}/Lib")
set(APPEND DXSDK_INCLUDE_PATH "$ENV{DXSDK_DIR}/Include")

set(SHADER_PATH "${GAME_MAIN_PATH}/shaders")
set(HLSL_INCLUDE_PATH "${SHADER_PATH}/include")
set(RESOURCES_PATH "${CMAKE_SOURCE_DIR}/res/")

#file search
set(GAME_ENGINE_SRC "${GAME_ENGINE_PATH}/include/Rice/GL/GraphicsManager.hpp" "${GAME_ENGINE_PATH}/src/GL/GraphicsManager.cpp")
set(GAME_MAIN_SRC "${GAME_MAIN_PATH}/src/test.cpp")
#file(GLOB_RECURSE GAME_ENGINE_SRC "${GAME_ENGINE_PATH}/*.h" "${GAME_ENGINE_PATH}/*.c" "${GAME_ENGINE_PATH}/*.hpp" "${GAME_ENGINE_PATH}/*.cpp")
#file(GLOB_RECURSE GAME_MAIN_SRC "${GAME_MAIN_PATH}/*.h" "${GAME_MAIN_PATH}/*.c" "${GAME_MAIN_PATH}/*.hpp" "${GAME_MAIN_PATH}/*.cpp")
file(GLOB_RECURSE LIBS_x64 "${CMAKE_SOURCE_DIR}/lib/win64/*.lib")
file(GLOB_RECURSE DLLs_x64 "${CMAKE_SOURCE_DIR}/lib/win64/*.dll")

file(GLOB_RECURSE HLSL_VS_SHADER_FILES "${SHADER_PATH}/vs/*.hlsl")
file(GLOB_RECURSE HLSL_PS_SHADER_FILES "${SHADER_PATH}/ps/*.hlsl")
file(GLOB_RECURSE RESOURCES "${RESOURCES_PATH}/*.*")
set(HLSL_ALL_SHADER_FILES ${HLSL_VS_SHADER_FILES} ${HLSL_PS_SHADER_FILES})

#global property
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -D UNICODE")

#Source config

#GameEngine

#includes
include_directories(GameEngine ${VULKAN_INCLUDE_PATH})
include_directories(GameEngine ${DXSDK_INCLUDE_PATH})
include_directories(GameEngine ${LIB_INCLUDE_PATH})
include_directories(GameEngine "${GAME_ENGINE_PATH}/include")
include_directories(GameEngine "${GAME_ENGINE_PATH}/src")

#build
add_library (GameEngine ${GAME_ENGINE_SRC})
target_precompile_headers(GameEngine PUBLIC "${GAME_ENGINE_PATH}/src/pch.h")

#GameMain
#includes
include_directories(GameMain ${VULKAN_INCLUDE_PATH})
include_directories(GameMain ${DXSDK_INCLUDE_PATH})
include_directories(GameMain ${LIB_INCLUDE_PATH})
include_directories(GameMain "${GAME_MAIN_PATH}/src")
#build
add_executable (GameMain ${GAME_MAIN_SRC})
target_precompile_headers(GameMain PUBLIC "${GAME_MAIN_PATH}/src/pch.h")


target_link_directories(GameMain PUBLIC
        "${CMAKE_SOURCE_DIR}/lib/win64"
        ${PROJECT_BINARY_DIR}
        "${DXSDK_LIB_PATH}/x64"
        ${VULKAN_LIB_PATH}
        )

target_link_libraries(GameMain
        "GameEngine"
        "glfw3"
        "vulkan-1"
        "vk-bootstrap"
        #"OpenAL32"
        "ogg"
        "vorbis"
        "vorbisfile"
        "vorbisenc"
        "SDL2"
	"SDL2main"
)

add_dependencies(GameMain GameEngine)



#copy dll
add_custom_command(TARGET GameMain POST_BUILD   # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND}
        -E copy_if_different                    # which executes "cmake - E copy_if_different..."
        ${DLLs_x64}                           # <--this is in-file
        ${CMAKE_BINARY_DIR})                  # <--this is out-file path

#copy resources
add_custom_target(Resources ALL DEPENDS ${RESOURCES})
set(OUT_RES_DIR ${CMAKE_BINARY_DIR}/resources)

file(MAKE_DIRECTORY ${OUT_RES_DIR})

#add_custom_command(TARGET Resources   # Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND}
#        -E copy_directory                    # which executes "cmake - E copy_if_different..."
#        ${RESOURCES_PATH}                           # <--this is in-file
#        ${OUT_RES_DIR})          # <--this is out-file path