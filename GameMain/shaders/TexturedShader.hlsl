struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float4 viewPos : POSITION0;
    float3 light : POSITION2;
    float3 world_pos : POSITION3;
    float3 world_norm : POSITION4;
};

Texture2D tex;
SamplerState samp;

float4 main(PixelShaderInput input) : SV_TARGET
{
    return tex.Sample(samp, input.world_pos.xz);
}