find_program(GLSLC glslc REQUIRED)

set(SHADER_OUT "${CMAKE_CURRENT_BINARY_DIR}")
file(MAKE_DIRECTORY "${SHADER_OUT}")

file(GLOB_RECURSE VERT_SHADER_FILES "*.vert")
file(GLOB_RECURSE FRAG_SHADER_FILES "*.frag")

set(TEST_SHADERS_TARGET TestShaders)
set(TEST_SHADER_SPV "")
add_custom_target(${TEST_SHADERS_TARGET} ALL)

foreach(FILE ${VERT_SHADER_FILES})
    get_filename_component(FILE_WE ${FILE} NAME_WE)
    set(OUT "${SHADER_OUT}/${FILE_WE}.vert.spv")
    add_custom_command(
        OUTPUT ${OUT}
        COMMAND ${GLSLC} ${FILE} -o ${OUT} -fshader-stage=vert
        DEPENDS ${FILE}
        COMMENT "VERT COMPILATION ${FILE}"
        VERBATIM)
    add_custom_target(${FILE_WE}_vert_spv DEPENDS ${OUT})
    add_dependencies(${TEST_SHADERS_TARGET} ${FILE_WE}_vert_spv)
    list(APPEND TEST_SHADER_SPV ${OUT})
endforeach()

foreach(FILE ${FRAG_SHADER_FILES})
    get_filename_component(FILE_WE ${FILE} NAME_WE)
    set(OUT "${SHADER_OUT}/${FILE_WE}.frag.spv")
    add_custom_command(
        OUTPUT ${OUT}
        COMMAND ${GLSLC} ${FILE} -o ${OUT} -fshader-stage=frag
        DEPENDS ${FILE}
        COMMENT "FRAG COMPILATION ${FILE}"
        VERBATIM)
    add_custom_target(${FILE_WE}_frag_spv DEPENDS ${OUT})
    add_dependencies(${TEST_SHADERS_TARGET} ${FILE_WE}_frag_spv)
    list(APPEND TEST_SHADER_SPV ${OUT})
endforeach()

# The samples load shaders as "shaders/<name>.spv" relative to the working
# directory, so they live next to the executables in bin/shaders.
set(TEST_SHADER_SPV ${TEST_SHADER_SPV} CACHE INTERNAL "")
install(FILES ${TEST_SHADER_SPV} DESTINATION bin/shaders)
