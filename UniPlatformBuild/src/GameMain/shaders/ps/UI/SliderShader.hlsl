struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float4 viewPos : POSITION0;
    float3 light : POSITION2;
    float3 world_pos : POSITION3;
    float3 world_norm : POSITION4;
    float2 texcoord : TEXCOORD1;
};

cbuffer Buf : register(b0)
{
    float4 data;
    float value;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 col = tex.Sample(samp, input.texcoord);
    float mask = col.w;
    float grayscale = dot(col.xyz, float3(0.21, +0.71, 0.07));
    if (input.texcoord.x > value)
    {
        col.w *= grayscale;
    }
    col.xyz = lerp(1, data.xyz, grayscale);
    col = lerp(col, grayscale, data.w);
    return col;
}