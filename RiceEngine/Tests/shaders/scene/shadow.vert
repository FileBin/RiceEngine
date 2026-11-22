#version 450

#extension GL_GOOGLE_include_directive : enable

/*
 * shadow.vert — the casters pass of the directional light's shadow map.
 *
 * Depth only: it is built with Shader::buildShadowPipeline() (no fragment stage,
 * no colour attachment, depth test + write) and renders every registered mesh
 * transformed by `world * light_wvp`. It reads the SAME per-mesh ModelData
 * uniform buffer (binding 0) as the scene draw — refreshed every frame in
 * RenderingMesh::updateConstBuffer() — and the scene LightData block (binding 2),
 * so the pass never has to be re-recorded when the camera moves.
 */

layout (location = 0) in vec3 inPosition;

// Per-mesh transform, binding 0: only `world` is used here.
layout(binding = 0) uniform Constants {
    mat4 world;
    mat4 world_view;
    mat4 projection;
} constants;

#include "light.glsl"

void main() {
    vec4 pos = vec4(inPosition, 1.0) * constants.world;
    pos = pos * light.light_wvp;

    // Same y flip as the scene vertex shader. With it the map is stored in the
    // D3D orientation (texel row 0 = ndc y +1), which is what the lookup in
    // shadow.glsl assumes when it maps lPos.y to -y/2 + 0.5.
    pos.y = -pos.y;

    gl_Position = pos;
}
