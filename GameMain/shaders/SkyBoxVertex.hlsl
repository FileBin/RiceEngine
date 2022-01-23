cbuffer ConstantBuffer
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
    float2 texcoord : POSITION1;
    float3 light : POSITION2;
};

PixelShaderInput main(float3 pos : POSITION, float4 norm : NORMAL, float2 texcoord : TEXCOORD)
{
    PixelShaderInput output = (PixelShaderInput) 0;

    float3 light = lightDirection;
    light = normalize(light);

    output.pos = float4(pos.xyz, 1.0f);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    norm.w = 0;
    
    output.norm = mul(norm, View);
    
    output.light = mul(light, View);
    
    output.viewPos = float4(pos.xyz, 1.0f);
    output.viewPos = mul(output.viewPos, View);
    
    output.texcoord = texcoord;

    return output;
}