#version 450
// M3 self-test, fragment stage: reads the compute shader's output before it
// shades, so a black frame means the draw saw a stale (or never written) SSBO -
// which is exactly what the readback asserts against.

layout(location = 0) in vec3 v_color;

layout(location = 0) out vec4 out_color;

layout(std430, binding = 0) readonly buffer Marker {
    uint values[64];
} marker;

void main() {
    // values[0] is 7 only after m3_setup.comp ran for this frame.
    vec3 color = (marker.values[0] == 7u) ? v_color : vec3(0.0);
    out_color = vec4(color, 1.0);
}
