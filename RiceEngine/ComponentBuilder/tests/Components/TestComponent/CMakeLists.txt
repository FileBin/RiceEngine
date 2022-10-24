cmake_minimum_required(VERSION 3.19)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

set(CMAKE_CURRENT_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.build")

set(COMPONENT_NAME ${PROJECT_NAME})
set(COMPONENT_NSP MyComponents)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

# file search
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl")

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} ${RICE_COMPONENT_BUILDER_LIBRARY_HELPER})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

include_directories(${PROJECT_NAME} ${RICE_COMPONENT_BUILDER_INCLUDES})
include_directories(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROJECT_NAME} ${METACOMPILER_INCLUDES})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${ANY_LIB_BIN_PATH}
    ${LIB_BIN_PATH}
    ${PROJECT_BINARY_DIR}
    ${DXSDK_LIB_PATH}/x64
    ${VULKAN_LIB_PATH}
)

add_compile_options(-ast-dump-xml)

target_link_libraries(${PROJECT_NAME} ${RICE_LIBS})
