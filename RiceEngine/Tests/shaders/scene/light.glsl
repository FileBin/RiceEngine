/*
 * light.glsl — the scene's directional light block.
 *
 * Set 0, binding 2 (Rice::Graphics::LightingBindings::kLightData), declared for
 * the vertex AND the fragment stage: the vertex shader needs light_wvp to
 * compute the light-space position, the fragment shader the ambient / diffuse /
 * direction terms.
 *
 * Only shaders compiled with -DRICE_LIGHTING include this; SceneRender binds
 * the block only for shaders whose Shader::addLighting() declared it.
 *
 * std140: the three vec3s each occupy a 16-byte slot, so the C++ side
 * (Rice::Graphics::LightData) stores them as Vector4f and the offsets are
 * 0 (light_wvp), 64 (ambient), 80 (diffuse), 96 (light_dir).
 */

layout(std140, binding = 2) uniform LightBlock {
    // world -> light clip space; multiplied by the world matrix, y is flipped in
    // the shaders the same way the camera projection is.
    mat4 light_wvp;
    // colour of a fully shadowed surface
    vec3 ambient;
    // colour of a fully lit surface
    vec3 diffuse;
    // direction the light rays travel, world space (normalised on the CPU)
    vec3 light_dir;
} light;
