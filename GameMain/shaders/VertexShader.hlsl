cbuffer ConstantBuffer : register(b0)
{
    float4x4 World;
    float4x4 WV;
    float4x4 Projection;
    float4x4 light_WVP;
}

#include "LightBuffer.hlsli"

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float4 viewPos : POSITION0;
    float4 lPos : POSITION1;
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
    output.lPos = mul(output.pos, light_WVP);
    output.world_pos = mul(output.pos, World);

    output.pos = mul(output.pos, WV);
    output.viewPos = output.pos;
    output.pos = mul(output.pos, Projection);

    output.world_norm = mul(norm.xyz, World);
    output.norm = mul(norm.xyz, WV);
    
    output.light = mul(light, WV);
   


    return output;
}