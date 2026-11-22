#version 450
// M3 self-test, vertex stage: draws whatever the compute pass wrote into the
// vertex buffer - nothing is set on the CPU (docs/tetrahedral-mesh.md §6.1).

layout(location = 0) in vec2 in_pos;

layout(location = 0) out vec3 v_color;

void main() {
    // Orange on purpose: the readback counts non-black pixels, and the scene is
    // cleared to black, so any count is the triangle's coverage.
    v_color = vec3(1.0, 0.5, 0.1);
    gl_Position = vec4(in_pos, 0.0, 1.0);
}
