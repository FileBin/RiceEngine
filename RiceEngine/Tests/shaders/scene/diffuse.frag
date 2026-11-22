#version 450

#extension GL_GOOGLE_include_directive : enable

#include "default.glsl"

#ifdef RICE_LIGHTING
// The 9x9 PCF and the directional light term (GetLight). Compiled into
// diffuse_lit.frag.spv only; the unlit scene shaders keep binding 0/1.
#include "shadow.glsl"
#endif

layout(std140, binding = 1) uniform fragmentUniforms {
    vec4 egst;
    vec4 color;
} ubo;

//output write
layout (location = 0) in FragShaderInput fragInput;
layout (location = 0) out vec4 outFragColor;

void main()
{
    vec3 n = fragInput.normal.xyz;

    vec4 color = ubo.color;
    vec4 egst = ubo.egst;

	float emission = egst.x;
    float glossines = egst.y;
    float specular = egst.z;
   
	vec3 eye = normalize(fragInput.view_pos.xyz);

	vec3 light = fragInput.light_dir.xyz;

	float em = emission;
	
	vec3 col = color.xyz;
	float dif = dot(-light, n);
	float spec = max(dot(reflect(light, n), -eye), 0);
	float shadow = 1;
	dif *= .5;
	dif += .5;
	spec = pow(spec, glossines + 1);
	dif = max(dif, em);
	em = 1 - em;
	col *= ((dif * shadow - 1) * em + 1);
	vec3 s = 1 - col;
	s *= spec;
    col += s * shadow * specular;

#ifdef RICE_LIGHTING
    // Directional light + shadow map: the result is the light colour
    // (diffuse where lit, ambient where shadowed), multiplied into the shading.
    col *= GetLight(fragInput.lPos);
#endif

    outFragColor = vec4(col, color.w);
}
