cmake_minimum_required(VERSION 3.19)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE TESTS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_subdirectory("shaders")

foreach(FILE ${TESTS})
    get_filename_component(TEST_NAME ${FILE} NAME_WE)

    project(${TEST_NAME})

    set(CMAKE_CXX_STANDARD 20)

    #file search

    add_executable (${PROJECT_NAME} ${FILE} ${HEADERS})
    target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h)

    include_directories(${PROJECT_NAME} ${RICE_INCLUDE_PATH})
    include_directories(${PROJECT_NAME} ${LIB_INCLUDE_PATH})
    include_directories(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/include)

    target_link_directories(${PROJECT_NAME} PUBLIC
            ${ANY_LIB_BIN_PATH}
            ${LIB_BIN_PATH}
            ${PROJECT_BINARY_DIR}
            ${DXSDK_LIB_PATH}/x64
            ${VULKAN_LIB_PATH}
            )


    target_link_libraries(${PROJECT_NAME}
        ${RICE_LIBS}
    )

    add_dependencies(${PROJECT_NAME} ${RICE_NAME} ${TEST_SHADERS_TARGET})
endforeach()

if(WIN32)
    #copy dll
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD   # Adds a post-build event to the target
        COMMAND ${CMAKE_COMMAND}
            -E copy_if_different                    # which executes "cmake - E copy_if_different..."
            ${RICE_DLLs_x64}                        # <--this is in-file
            ${CMAKE_BINARY_DIR})                    # <--this is out-file path
endif(WIN32)

