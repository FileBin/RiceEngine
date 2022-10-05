cmake_minimum_required(VERSION 3.19)

project("Rice")

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("lib")

set(RICE_NAME ${PROJECT_NAME} CACHE INTERNAL "")
set(RICE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include; ${LIB_INCLUDE_PATH} CACHE INTERNAL "")

add_subdirectory("Math")
add_subdirectory("Util")
add_subdirectory("GL")
add_subdirectory("Networking")

#file search
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library (${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_precompile_headers(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")

set(RICE_INCLUDE_PATH 
    ${RICE_INCLUDE_PATH};
    ${RICE_GL_INCLUDE_PATH};
    ${RICE_MATH_INCLUDE_PATH};
    ${RICE_UTIL_INCLUDE_PATH};
    ${RICE_NETWORKING_INCLUDE_PATH};
CACHE INTERNAL "")

include_directories(${PROJECT_NAME} ${RICE_INCLUDE_PATH})
include_directories(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_dependencies(${PROJECT_NAME} ${RICE_GL_NAME})
add_dependencies(${PROJECT_NAME} ${RICE_MATH_NAME})
add_dependencies(${PROJECT_NAME} ${RICE_UTIL_NAME})
add_dependencies(${PROJECT_NAME} ${RICE_NETWORKING_NAME})

set(VLK_NAME "vulkan")

if(WIN32)
    set(VLK_NAME "vulkan-1")
endif(WIN32)

set(OTHER_LIBS "")

if(UNIX)
    set(OTHER_LIBS "dl")
endif(UNIX)

set(RICE_EXT_LIBS 
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
CACHE INTERNAL "")

set(RICE_LIBS
    ${RICE_NAME} 
    ${RICE_GL_NAME} 
    ${RICE_MATH_NAME} 
    ${RICE_UTIL_NAME}
    ${RICE_NETWORKING_NAME}
    ${RICE_EXT_LIBS}
CACHE INTERNAL "")

add_subdirectory("Tests")
add_subdirectory("compcompiler")

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)