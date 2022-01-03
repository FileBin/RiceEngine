cbuffer ConstantBuffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
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

PixelShaderInput main(float3 pos : POSITION, float4 norm : NORMAL)
{
    PixelShaderInput output = (PixelShaderInput) 0;

    float3 light = float3(2, -5, -1);
    light = normalize(light);
    
    float4x4 WV = mul(World, View);
    
    output.light = mul(light, WV);

    output.pos = float4(pos.xyz, 1.0f);
    output.pos = mul(output.pos, World);
    output.world_pos = output.pos;

    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    output.norm = mul(float4(0, 1, 0, 0), WV);
    output.xAxis = mul(float4(1, 0, 0, 0), WV);
    output.zAxis = mul(float4(0, 0, 1, 0), WV);
    
    output.viewPos = float4(pos.xyz, 1.0f);
    output.viewPos = mul(output.viewPos, WV);

    return output;
}