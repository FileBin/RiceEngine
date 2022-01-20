#include "Shadows.hlsli"

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float4 viewPos : POSITION0;
    float4 lPos : TEXCOORD0;
    float3 light : POSITION1;
    float3 world_pos : POSITION2;
};
cbuffer CBuffer
{
    float time;
    float4 incolor;
    float4 egst;
}

float rand(float w_x, float w_y)
{
    float x = sin(dot(float2(w_x, w_y), float2(12.9898, 78.233)));
    return x - floor(x);
}

float4 main(PixelShaderInput input) : SV_TARGET {
    float3 n = input.norm;

	float emission = egst.x;
    float glossines = egst.y;
    float specular = egst.z;
   
	float3 eye = normalize(input.viewPos.xyz);

	float3 light = input.light;

	float em = emission;
	
	float3 col = incolor.xyz;
	float dif = dot(-light, n);
	float spec = max(dot(reflect(light, n), -eye), 0);
	float shadow = 1;
	dif *= .5;
	dif += .5;
	spec = pow(spec, glossines + 1);
	dif = max(dif, em);
	em = 1 - em;
	col *= ((dif * shadow - 1) * em + 1);
	float3 s = 1 - col;
	s *= spec;
    col += s * shadow * specular;
    
    col *= GetLight(input.lPos);

    return float4(col, incolor.w);
}