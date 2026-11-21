file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

add_library(RiceMath ${SOURCES} ${HEADERS})
add_library(Rice::Math ALIAS RiceMath)
rice_force_include(RiceMath "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")
target_link_libraries(RiceMath PRIVATE rice_headers)
target_include_directories(RiceMath
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Util/include"
)

install(TARGETS RiceMath DESTINATION lib)
install(DIRECTORY include DESTINATION .)
