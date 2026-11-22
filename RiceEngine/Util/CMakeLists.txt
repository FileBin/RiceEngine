file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

add_library(RiceUtil ${SOURCES} ${HEADERS})
add_library(Rice::Util ALIAS RiceUtil)
rice_force_include(RiceUtil "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")
target_link_libraries(RiceUtil PRIVATE rice_headers)
target_include_directories(RiceUtil PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

install(TARGETS RiceUtil DESTINATION lib)
install(DIRECTORY include DESTINATION .)
