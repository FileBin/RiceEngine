cbuffer ConstantBuffer : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
}

#include "LightBuffer.hlsli"

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float4 viewPos : POSITION0;
    float4 lPos :TEXCOORD0;
    float3 light : POSITION2;
    float3 world_pos : POSITION3;
    float3 world_norm : POSITION4;
};

PixelShaderInput main(float3 pos : POSITION, float4 norm : NORMAL)
{
    PixelShaderInput output = (PixelShaderInput) 0;

    float3 light = float3(2, -5, -1);
    light = normalize(light);

    output.pos = float4(pos.xyz, 1.0f);
    output.pos = mul(output.pos, World);
    output.world_pos = output.pos;
    output.lPos = mul(output.pos - norm*.002f, LVP); //Shadow Projection With Normal Bias

    output.pos = mul(output.pos, View);
    output.viewPos = output.pos;
    output.pos = mul(output.pos, Projection);

    output.norm = mul(norm.xyz, World);
    output.world_norm = output.norm;
    output.norm = mul(output.norm, View);
    
    float3x4 WV = mul(World, View);
    output.light = mul(light, WV);
   


    return output;
}