file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp" "include/*.inl")

add_library(RiceGL ${SOURCES} ${HEADERS})
add_library(Rice::GL ALIAS RiceGL)
rice_force_include(RiceGL "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")
target_include_directories(RiceGL PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Vulkan-Hpp lives in vulkan-headers, vk-bootstrap and fmt come from rice_deps.
target_link_libraries(RiceGL
    PUBLIC Rice::Math Rice::Util rice_deps rice_headers
    PRIVATE rice_deps
)

install(TARGETS RiceGL DESTINATION lib)
install(DIRECTORY include DESTINATION .)
