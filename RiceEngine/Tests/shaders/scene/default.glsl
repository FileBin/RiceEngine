struct FragShaderInput {
    vec4 normal;
    vec4 texcoord;
    vec4 light_dir;
    vec4 world_pos;
    vec4 view_pos;
#ifdef RICE_LIGHTING
    // Light-space position (world_pos * light.light_wvp). The fragment shader
    // turns it into the shadow map lookup (GetLight in shadow.glsl). Only
    // present in the lit variant of the scene shaders.
    vec4 lPos;
#endif
};
