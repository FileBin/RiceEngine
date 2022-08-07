#version 450

#extension GL_GOOGLE_include_directive : enable

#include "default.glsl"

layout(binding = 1) uniform fragmentUniforms {
    vec4 color;
} ubo;

//output write
layout (location = 0) in FragShaderInput fragInput;
layout (location = 0) out vec4 outFragColor;

void main()
{
    vec3 n = fragInput.normal.xyz;

    //return LoadShadowMap(fragInput.pos.xy);
    //TODO move thi into uniform buffer
    vec4 color = ubo.color; //vec4(0.95, 0.07, 0.02, 1.0);
    vec4 egst = vec4(0.3, 0.5, 0.6, 0.0);

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
    
    //TODO col *= getLight(fragInput.lPos);
    //outFragColor = ubo.color;
    outFragColor = vec4(col, color.w);
}