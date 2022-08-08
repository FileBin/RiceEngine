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

void main() {
	vec4 pos = vec4(inPosition, 1.0);

    fragOutput.world_pos = pos * constants.world;

    pos = pos * constants.world_view;
    fragOutput.view_pos = pos;

    fragOutput.normal = vec4(normalize(inNormal), 0) * constants.world_view;

    const vec4 light_dir = vec4(2, -4, 3, 0);

    fragOutput.light_dir = normalize(light_dir) * constants.world_view;
    fragOutput.texcoord.xy = inTexCoord;

    pos = pos * constants.projection;
    //flip y axis because opengl has y as down, but vulkan has y as up
    pos.y = -pos.y;
    gl_Position = pos;
}
