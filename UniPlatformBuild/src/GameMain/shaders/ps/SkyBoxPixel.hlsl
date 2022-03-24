struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float4 viewPos : POSITION0;
    float2 texcoord : POSITION1;
    float3 light : POSITION2;
};

Texture2D tex;
SamplerState samp;

float4 main(PixelShaderInput input) : SV_TARGET {
    float4 texcol = tex.Sample(samp, input.texcoord);
    
    float sunf = dot(input.light, -normalize(input.viewPos.xyz));
    sunf -= .95;
    sunf *= 30.;
    sunf = saturate(sunf);
    
    texcol = lerp(texcol, 1.f, sunf);
    
    return texcol; //float4(input.texcoord, .0f, 1.0f);
}