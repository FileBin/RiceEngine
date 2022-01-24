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
    float4 lPos : TEXCOORD0;
    float3 light : POSITION1;
    float3 world_pos : POSITION2;
    float2 texcoord : TEXCOORD1;
};

PixelShaderInput main(float3 pos : POSITION, float4 norm : NORMAL, float2 texcoord : TEXCOORD)
{
    PixelShaderInput output = (PixelShaderInput) 0;

    float3 light = lightDirection;
    light = normalize(light);

    output.pos = float4(pos.xyz, 1.0f);
    output.lPos = mul(output.pos, light_WVP);
    output.world_pos = mul(output.pos, World);

    output.pos = mul(output.pos, WV);
    output.viewPos = output.pos;
    output.pos = mul(output.pos, Projection);

    output.norm = mul(norm.xyz, WV);
    
    output.light = mul(light, WV);
   
    output.texcoord = texcoord;

    return output;
}