cmake_minimum_required(VERSION 3.19)

find_program(GLSLC glslc)

set(SHADER_PATH "${CMAKE_SOURCE_DIR}/shaders")

file(MAKE_DIRECTORY ${OUT_SHADERS_DIR})

file (GLOB_RECURSE FRAG_SHADER_FILES"${SHADER_PATH}/*.frag")
file (GLOB_RECURSE VERT_SHADER_FILES"${SHADER_PATH}/*.vert")

message("vert shaders: ${VERT_SHADER_FILES}")
message("frag shaders: ${FRAG_SHADER_FILES}")

set(ALL_SHADER_FILES ${FRAG_SHADER_FILES} ${VERT_SHADER_FILES})

add_custom_target(Shaders ALL DEPENDS ${ALL_SHADER_FILES})
set(OUT_SHADERS_DIR ${PROJECT_BINARY_DIR}/shaders)

#shader compilation
foreach(FILE ${VERT_SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  add_custom_command(TARGET Shaders
    COMMAND ${GLSLC} ${FILE} -o ${OUT_SHADERS_DIR}/${FILE_WE}.vert.spv -fshader-stage=vert
    MAIN_DEPENDENCY ${FILE}
    COMMENT "VERT COMPILATION ${FILE}")
endforeach(FILE)

foreach(FILE ${FRAG_SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  add_custom_command(TARGET Shaders
    COMMAND ${GLSLC} ${FILE} -o ${OUT_SHADERS_DIR}/${FILE_WE}.frag.spv -fshader-stage=frag
    MAIN_DEPENDENCY ${FILE}
    COMMENT "FRAG COMPILATION ${FILE}")
endforeach(FILE)