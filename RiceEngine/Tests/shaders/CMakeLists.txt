cmake_minimum_required(VERSION 3.19)

find_program(GLSLC glslc)

set(SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(OUT_SHADER_PATH ${CMAKE_CURRENT_BINARY_DIR})

file(MAKE_DIRECTORY ${OUT_SHADER_PATH})

file (GLOB_RECURSE FRAG_SHADER_FILES "${SHADER_PATH}/*.frag")
file (GLOB_RECURSE VERT_SHADER_FILES "${SHADER_PATH}/*.vert")

message("vert shaders: ${VERT_SHADER_FILES}")
message("frag shaders: ${FRAG_SHADER_FILES}")

set(ALL_SHADER_FILES ${FRAG_SHADER_FILES} ${VERT_SHADER_FILES})

set(TEST_SHADERS_TARGET TestShaders CACHE INTERNAL "")

add_custom_target(${TEST_SHADERS_TARGET} ALL DEPENDS ${ALL_SHADER_FILES})
#shader compilation
foreach(FILE ${VERT_SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  add_custom_command(TARGET ${TEST_SHADERS_TARGET}
    COMMAND ${GLSLC} ${FILE} -o ${OUT_SHADER_PATH}/${FILE_WE}.vert.spv -fshader-stage=vert
    MAIN_DEPENDENCY ${FILE}
    COMMENT "VERT COMPILATION ${FILE}")
endforeach(FILE)

foreach(FILE ${FRAG_SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  add_custom_command(TARGET ${TEST_SHADERS_TARGET}
    COMMAND ${GLSLC} ${FILE} -o ${OUT_SHADER_PATH}/${FILE_WE}.frag.spv -fshader-stage=frag
    MAIN_DEPENDENCY ${FILE}
    COMMENT "FRAG COMPILATION ${FILE}")
endforeach(FILE)