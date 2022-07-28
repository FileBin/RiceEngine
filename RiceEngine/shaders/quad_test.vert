#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 outColor;

layout(binding = 0) uniform Constants {
    mat4 render_matrix;
} constants;

void main() {
	//output the position of each vertex
	gl_Position = constants.render_matrix * vec4(inPosition, 1.0f);
	outColor = inColor;
}
