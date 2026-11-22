file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

add_library(RiceNetworking ${SOURCES} ${HEADERS})
add_library(Rice::Networking ALIAS RiceNetworking)
rice_force_include(RiceNetworking "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")
target_include_directories(RiceNetworking PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(RiceNetworking PUBLIC Rice::Math Rice::Util Rice::GL rice_deps rice_headers)

install(TARGETS RiceNetworking DESTINATION lib)
install(DIRECTORY include DESTINATION .)
