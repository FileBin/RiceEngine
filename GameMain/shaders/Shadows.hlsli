Texture2D shadowAtlas : register(t13);
SamplerState shadowSampler : register(s13);


#define SHADOWMAP_SIZE 1024

#include "LightBuffer.hlsli"

float SampleShadowMap(float2 pos)
{
    return shadowAtlas.Sample(shadowSampler, pos).x;
}

float3 GetLight(float4 lPos)
{
    float shadow = 0.f;
    
    lPos.xyz /= lPos.w;
    
    shadow = 1 - saturate(max(abs(lPos.x), abs(lPos.y))*4-3);
    
    lPos.x = lPos.x / 2 + 0.5;
    lPos.y = lPos.y / -2 + 0.5;
    float2 pixelCoords = lPos.xy;
    
    float e = .004f;
    float samples = 8;
   
    
    float fac = 0;
    
    [unroll(17)]
    for (float i = -e; i <= e; i += e / samples)
    {
        [unroll(17)]
        for (float j = -e; j <= e; j += e/samples)
        {
            float factor = SampleShadowMap(pixelCoords + float2(i, j)) + .005f - lPos.z;
            factor = saturate(factor * 300);
            fac += factor;
        }
    }
    
    fac /= pow(2 * samples + 1, 2);
    
    shadow *= saturate(1 - fac);
   
    return lerp(diff.xyz, amb.xyz, shadow);

}