cmake_minimum_required(VERSION 3.19)

project("Rice")

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("lib")
add_subdirectory("shaders")

set(RICE_NAME ${PROJECT_NAME} CACHE INTERNAL "")
set(RICE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIB_INCLUDE_PATH} CACHE INTERNAL "")

add_subdirectory("Math")
add_subdirectory("Util")
add_subdirectory("GL")

#file search
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library (${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_precompile_headers(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")

set(VULKAN_LIB_PATH "$ENV{VULKAN_SDK}/lib")
set(VULKAN_INCLUDE_PATH "$ENV{VK_SDK_PATH}/Include")

set(APPEND DXSDK_LIB_PATH "$ENV{DXSDK_DIR}/Lib")
set(APPEND DXSDK_INCLUDE_PATH "$ENV{DXSDK_DIR}/Include")

set(RICE_INCLUDE_PATH 
    ${RICE_INCLUDE_PATH}
    ${RICE_GL_INCLUDE_PATH}
    ${RICE_MATH_INCLUDE_PATH}
    ${RICE_UTIL_INCLUDE_PATH}
)

include_directories(${PROJECT_NAME} ${RICE_INCLUDE_PATH})
include_directories(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_dependencies(${PROJECT_NAME} ${RICE_GL_NAME})
add_dependencies(${PROJECT_NAME} ${RICE_MATH_NAME})
add_dependencies(${PROJECT_NAME} ${RICE_UTIL_NAME})
add_dependencies(${PROJECT_NAME} ${})

set(RICE_LIBS
    ${RICE_NAME} 
    ${RICE_GL_NAME} 
    ${RICE_MATH_NAME} 
    ${RICE_UTIL_NAME}
CACHE INTERNAL "")