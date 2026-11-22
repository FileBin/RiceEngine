file(GLOB_RECURSE TEST_HEADERS "include/*.h" "include/*.hpp")
file(GLOB_RECURSE TEST_SOURCES "src/*.cpp" "src/*.c")

add_subdirectory("shaders")

# Samples that no longer match the engine API. TriangleTest drives Window and
# GraphicsManager directly through the removed new_ref()/release() API and is
# superseded by QuadTest/CubeTest, which go through Scene + SceneRender.
set(RICE_SKIP_TESTS TriangleTest)

# Every .cpp under src/ becomes its own executable (WindowTest, QuadTest,
# CubeTest, MultiCubePerformanceTest, CoreTest, ...).
foreach(FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${FILE} NAME_WE)

    if(TEST_NAME IN_LIST RICE_SKIP_TESTS)
        message(STATUS "Test ${TEST_NAME} skipped (outdated API)")
        continue()
    endif()

    add_executable(${TEST_NAME} ${FILE} ${TEST_HEADERS})
    rice_force_include(${TEST_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")
    target_include_directories(${TEST_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_link_libraries(${TEST_NAME} PRIVATE Rice rice_deps)

    # 16-byte compare-and-swap for the lock-free containers (x86-64 only).
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-mcx16" RICE_HAS_MCX16)
    if(RICE_HAS_MCX16 AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
        target_compile_options(${TEST_NAME} PRIVATE -mcx16)
    endif()

    add_dependencies(${TEST_NAME} ${TEST_SHADERS_TARGET})
    install(TARGETS ${TEST_NAME} RUNTIME DESTINATION bin)
endforeach()
