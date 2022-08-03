#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 outColor;

layout(binding = 0) uniform Constants {
    mat4 world;
    mat4 view;
    mat4 projection;
} constants;

void main() {
	//output the position of each vertex
	vec4 pos = vec4(inPosition, 1.0);
    pos = pos * constants.world;
    pos = pos * constants.view;
    pos = pos * constants.projection;
    pos.y = -pos.y;
    gl_Position = pos;
    
	outColor = inColor;
}
