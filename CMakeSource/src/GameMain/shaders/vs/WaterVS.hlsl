cbuffer ConstantBuffer : register(b0)
{
    float4x4 World;
    float4x4 WV;
    float4x4 Projection;
}

#include "LightBuffer.hlsli"

cbuffer CBuffer : register(b1)
{
    float time;
    float4 Resolution;
}

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float4 viewPos : POSITION0;
    float3 xAxis : POSITION1;
    float3 zAxis : POSITION2;
    float3 light : POSITION3;
    float3 world_pos : POSITION4;
};

float _rand(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

float rand(float2 uv)
{
    uv %= 256;
    float t = max(uv.x, uv.y);
    t = lerp(t, min(uv.x, uv.y), .5);
    return lerp(_rand(uv), _rand(uv + 256), .5);
}

float noise(float2 pos)
{
    float2 fpos = floor(pos);
    float2 v = float2(0, 1);
    float2x2 samples;
    samples._11 = rand(fpos);
    samples._12 = rand(fpos + v.yx);
    samples._21 = rand(fpos + v.xy);
    samples._22 = rand(fpos + v.yy);
    float2 t = pos - fpos;
    t = .5 - cos(t * 3.141592) * .5;
    return (samples._11 * (1 - t.x) + samples._12 * t.x) * (1 - t.y) + (samples._21 * (1 - t.x) + samples._22 * t.x) * t.y;
}

float simplexNoise(float2 uv)
{
    float val = 0;
    val += noise(uv);
    return val;
}

PixelShaderInput main(float3 pos : POSITION, float4 norm : NORMAL)
{
    PixelShaderInput output = (PixelShaderInput) 0;

    float3 light = lightDirection;
    light = normalize(light);
    
    output.light = mul(float4(light, 0), WV);
    
    float4 p = float4(pos.xyz, 1);
    float4 wp = mul(p, World);
    p.y -= simplexNoise(wp.xz * .5 + time.xx * .4);

    output.pos = p;
    output.pos = mul(output.pos, WV);
    output.world_pos = wp;
    output.pos = mul(output.pos, Projection);

    output.norm = mul(float4(0, 1, 0, 0), WV);
    output.xAxis = mul(float4(1, 0, 0, 0), WV);
    output.zAxis = mul(float4(0, 0, 1, 0), WV);
    
    output.viewPos = float4(pos.xyz, 1.0f);
    output.viewPos = mul(output.viewPos, WV);

    return output;
}