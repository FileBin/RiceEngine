#version 450

#extension GL_GOOGLE_include_directive : enable

#include "default.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

layout (location = 0) out FragShaderInput fragOutput;

layout(binding = 0) uniform Constants {
    mat4 world;
    mat4 world_view;
    mat4 projection;
} constants;

#ifdef RICE_LIGHTING
// The scene's directional light: set 0, binding 2 (see light.glsl).
// Compiled into default_lit.vert.spv only, together with Shader::addLighting(),
// so the unlit scene shaders keep their binding 0/1 pipeline layout.
#include "light.glsl"
#endif

void main() {
	vec4 pos = vec4(inPosition, 1.0);

    fragOutput.world_pos = pos * constants.world;

#ifdef RICE_LIGHTING
    // Light-space position for the shadow lookup: the fragment shader divides
    // by w and samples the map (GetLight in shadow.glsl).
    fragOutput.lPos = fragOutput.world_pos * light.light_wvp;
#endif

    pos = pos * constants.world_view;
    fragOutput.view_pos = pos;

    fragOutput.normal = vec4(normalize(inNormal), 0) * constants.world_view;

#ifdef RICE_LIGHTING
    fragOutput.light_dir = vec4(normalize(light.light_dir), 0) * constants.world_view;
#else
    const vec4 light_dir = vec4(2, -4, 3, 0);

    fragOutput.light_dir = normalize(light_dir) * constants.world_view;
#endif
    fragOutput.texcoord.xy = inTexCoord;

    pos = pos * constants.projection;
    //flip y axis because opengl has y as down, but vulkan has y as up
    pos.y = -pos.y;
    gl_Position = pos;
}
