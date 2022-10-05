cmake_minimum_required(VERSION 3.19)

project("compcompiler")

set(RICE_GL_NAME ${PROJECT_NAME} CACHE INTERNAL "")
set(RICE_GL_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 20)

#file search
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl" )

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

include_directories(${PROJECT_NAME} ${RICE_INCLUDE_PATH})
include_directories(${PROJECT_NAME} ${LIB_INCLUDE_PATH})
include_directories(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_dependencies(${PROJECT_NAME} ${RICE_NAME} ${TEST_SHADERS_TARGET})

add_subdirectory("tests")

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)