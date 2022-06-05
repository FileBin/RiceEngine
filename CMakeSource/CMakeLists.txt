cmake_minimum_required (VERSION 3.8)

find_path(BETTER_CPP_INCLUDE_PATH REQUIRED)
find_path(BETTER_CPP_LIB_PATH REQUIRED)

#project config
project ("GameEngine")
project ("GameMain")

set(GAME_ENGINE_PATH "${CMAKE_SOURCE_DIR}/src/RiceEngine")
set(GAME_MAIN_PATH "${CMAKE_SOURCE_DIR}/src/GameMain")

set(LIB_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/lib/include")
set(LIB_BIN_PATH "${CMAKE_SOURCE_DIR}/lib/bin")

set(LINKER_FLAGS "")

if(WIN32)
    set(LIB_BIN_PATH "${LIB_BIN_PATH}/win64")
endif(WIN32)
if(UNIX)
    set(LINKER_FLAGS "-no-pie")
    set(LIB_BIN_PATH "${LIB_BIN_PATH}/linux64")
endif(UNIX)

set(ANY_LIB_BIN_PATH "${LIB_BIN_PATH}/any")

message("lib any path: ${ANY_LIB_BIN_PATH}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -D UNICODE ${LINKER_FLAGS}")
  	set(LIB_BIN_PATH "${LIB_BIN_PATH}/clang")
  	
  	
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -D UNICODE ${LINKER_FLAGS}")
  	set(LIB_BIN_PATH "${LIB_BIN_PATH}/gnu")
  	
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  	set(LIB_BIN_PATH "${LIB_BIN_PATH}/i8")
  	
  	
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/W4 /WX-)
  	set(LIB_BIN_PATH "${LIB_BIN_PATH}/msvc")
endif()

message("lib compiler path: ${LIB_BIN_PATH}")

set(VULKAN_LIB_PATH "$ENV{VULKAN_SDK}/lib")
set(VULKAN_INCLUDE_PATH "$ENV{VK_SDK_PATH}/Include")

set(APPEND DXSDK_LIB_PATH "$ENV{DXSDK_DIR}/Lib")
set(APPEND DXSDK_INCLUDE_PATH "$ENV{DXSDK_DIR}/Include")

set(SHADER_PATH "${CMAKE_SOURCE_DIR}/shaders")
set(RESOURCES_PATH "${CMAKE_SOURCE_DIR}/res/")

#file search

#for test use only needed files
set(GAME_ENGINE_SRC 
	"${GAME_ENGINE_PATH}/include/Rice/GL/GraphicsManager.hpp"	"${GAME_ENGINE_PATH}/src/Engine/InputManager.cpp"
	"${GAME_ENGINE_PATH}/src/Engine/Window.cpp"
	"${GAME_ENGINE_PATH}/src/Engine/Log.cpp"
	"${GAME_ENGINE_PATH}/src/Math/Vectors.cpp"
	"${GAME_ENGINE_PATH}/src/Math/Math.cpp"
	"${GAME_ENGINE_PATH}/src/Math/Matrixes.cpp"
	"${GAME_ENGINE_PATH}/src/GL/API_helper.cpp"
	"${GAME_ENGINE_PATH}/src/GL/Shader.cpp"
	"${GAME_ENGINE_PATH}/src/GL/GraphicsComponentBase.cpp"
	"${GAME_ENGINE_PATH}/src/Util/Util.cpp"
 	"${GAME_ENGINE_PATH}/src/GL/GraphicsManager.cpp"
 	"${GAME_ENGINE_PATH}/src/GL/Buffer.cpp"
 	"${GAME_ENGINE_PATH}/src/GL/VertexBuffer.cpp"
 	"${GAME_ENGINE_PATH}/src/GL/IndexBuffer.cpp"
 	"${GAME_ENGINE_PATH}/src/GL/CommandBuffer.cpp")
 
set(GAME_MAIN_SRC "${GAME_MAIN_PATH}/src/test.cpp")

#file(GLOB_RECURSE GAME_ENGINE_SRC "${GAME_ENGINE_PATH}/*.h" "${GAME_ENGINE_PATH}/*.c" "${GAME_ENGINE_PATH}/*.hpp" "${GAME_ENGINE_PATH}/*.cpp")
#file(GLOB_RECURSE GAME_MAIN_SRC "${GAME_MAIN_PATH}/*.h" "${GAME_MAIN_PATH}/*.c" "${GAME_MAIN_PATH}/*.hpp" "${GAME_MAIN_PATH}/*.cpp")

file(GLOB_RECURSE DLLs_x64 "${CMAKE_SOURCE_DIR}/lib/*.dll")

file(GLOB_RECURSE FRAG_SHADER_FILES
    "${SHADER_PATH}/*.frag"
    )
file (GLOB_RECURSE VERT_SHADER_FILES
    "${SHADER_PATH}/*.vert")

    message("vert shaders: ${VERT_SHADER_FILES}")
    message("frag shaders: ${FRAG_SHADER_FILES}")


file(GLOB_RECURSE RESOURCES "${RESOURCES_PATH}/*.*")

#global property
set(CMAKE_CXX_STANDARD 20)

#Source config

message("bettercpp: ${BETTER_CPP_INCLUDE_PATH}")

#GameEngine
include_directories(GameEngine ${VULKAN_INCLUDE_PATH})
include_directories(GameEngine ${DXSDK_INCLUDE_PATH})
include_directories(GameEngine ${LIB_INCLUDE_PATH})
include_directories(GameEngine ${BETTER_CPP_INCLUDE_PATH})
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
include_directories(GameMain ${BETTER_CPP_INCLUDE_PATH})
include_directories(GameMain "${GAME_MAIN_PATH}/src")
#build
add_executable (GameMain ${GAME_MAIN_SRC})
target_precompile_headers(GameMain PUBLIC "${GAME_MAIN_PATH}/src/pch.h")

target_link_directories(GameMain PUBLIC
        ${ANY_LIB_BIN_PATH}
        ${LIB_BIN_PATH}
        ${PROJECT_BINARY_DIR}
        "${DXSDK_LIB_PATH}/x64"
        ${VULKAN_LIB_PATH}
        ${BETTER_CPP_LIB_PATH}
        )

set(VLK_NAME "vulkan")

if(WIN32)
    set(VLK_NAME "vulkan-1")
endif(WIN32)

set(OTHER_LIBS "")

if(UNIX)
    set(OTHER_LIBS "dl")
endif(UNIX)


target_link_libraries(GameMain
    "GameEngine"
    "BetterCpp"
    "${VLK_NAME}"
    "vk-bootstrap"
    #"OpenAL32"
    "fmt"
    "ogg"
    "vorbis"
    "vorbisfile"
    "SDL2"
    "SDL2main"
	${OTHER_LIBS}
)

add_dependencies(GameMain GameEngine)

find_program(GLSLC glslc)

set(OUT_SHADERS_DIR "${PROJECT_BINARY_DIR}/shaders")

file(MAKE_DIRECTORY ${OUT_SHADERS_DIR})

#shader compilation
foreach(FILE ${VERT_SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  add_custom_command(TARGET GameMain POST_BUILD
    COMMAND ${GLSLC} ${FILE} -o "${OUT_SHADERS_DIR}/${FILE_WE}.vert.spv" -fshader-stage=vert
    MAIN_DEPENDENCY ${FILE}
    COMMENT "VERT ${FILE}")
endforeach(FILE)

foreach(FILE ${FRAG_SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  add_custom_command(TARGET GameMain POST_BUILD
    COMMAND ${GLSLC} ${FILE} -o "${OUT_SHADERS_DIR}/${FILE_WE}.frag.spv" -fshader-stage=frag
    MAIN_DEPENDENCY ${FILE}
    COMMENT "FRAG ${FILE}")
endforeach(FILE)

if(WIN32)
#copy dll
add_custom_command(TARGET GameMain POST_BUILD   # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND}
        -E copy_if_different                    # which executes "cmake - E copy_if_different..."
        ${DLLs_x64}                             # <--this is in-file
        ${CMAKE_BINARY_DIR})                    # <--this is out-file path

endif(WIN32)
#copy resources
add_custom_target(Resources ALL DEPENDS ${RESOURCES})
set(OUT_RES_DIR ${CMAKE_BINARY_DIR}/resources)

file(MAKE_DIRECTORY ${OUT_RES_DIR})

add_custom_command(TARGET Resources   # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND}
        -E copy_directory             # which executes "cmake - E copy_if_different..."
        ${RESOURCES_PATH}             # <--this is in-file
        ${OUT_RES_DIR})          	  # <--this is out-file path
