file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

add_library(Rice ${SOURCES} ${HEADERS})
add_library(Rice::Engine ALIAS Rice)
rice_force_include(Rice "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")

# The engine library re-exports the include dirs of every module it is built
# from, so a consumer only has to link Rice.
target_include_directories(Rice
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../GL/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Math/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Util/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Networking/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(Rice
    PUBLIC
        Rice::GL Rice::Math Rice::Util Rice::Networking rice_deps rice_headers
    PRIVATE
        ${CMAKE_DL_LIBS}
)

install(TARGETS Rice DESTINATION lib)
install(DIRECTORY include DESTINATION .)
