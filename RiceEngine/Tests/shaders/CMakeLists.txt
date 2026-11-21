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

# The lit variants of the scene shaders: the same sources compiled with
# -DRICE_LIGHTING, which adds the scene light block (binding 2) and the shadow
# map (binding 3) to them. They exist as separate SPIR-V files so that the plain
# default.vert.spv / diffuse.frag.spv keep a pipeline layout of just bindings
# 0/1 - the unlit demos are compiled against those and must not change.
#
# The include files have to be dependencies: touching light.glsl or shadow.glsl
# has to rebuild the two variants.
set(SCENE_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/scene/default.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/scene/light.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/scene/shadow.glsl")

set(LIT_SHADERS
    "scene/default.vert:default_lit:vert"
    "scene/diffuse.frag:diffuse_lit:frag")

foreach(ENTRY ${LIT_SHADERS})
    string(REPLACE ":" ";" PARTS "${ENTRY}")
    list(GET PARTS 0 SRC_REL)
    list(GET PARTS 1 NAME)
    list(GET PARTS 2 STAGE)
    set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_REL}")
    set(OUT "${SHADER_OUT}/${NAME}.${STAGE}.spv")
    add_custom_command(
        OUTPUT ${OUT}
        COMMAND ${GLSLC} ${SRC} -DRICE_LIGHTING=1 -o ${OUT} -fshader-stage=${STAGE}
        DEPENDS ${SRC} ${SCENE_INCLUDES}
        COMMENT "LIT COMPILATION ${SRC_REL}"
        VERBATIM)
    add_custom_target(${NAME}_${STAGE}_spv DEPENDS ${OUT})
    add_dependencies(${TEST_SHADERS_TARGET} ${NAME}_${STAGE}_spv)
    list(APPEND TEST_SHADER_SPV ${OUT})
endforeach()

# The samples load shaders as "shaders/<name>.spv" relative to the working
# directory, so they live next to the executables in bin/shaders.
set(TEST_SHADER_SPV ${TEST_SHADER_SPV} CACHE INTERNAL "")
install(FILES ${TEST_SHADER_SPV} DESTINATION bin/shaders)
