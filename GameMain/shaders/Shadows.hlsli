Texture2D shadowAtlas : register(t13);
SamplerState shadowSampler : register(s13);


#define SHADOWMAP_SIZE 1024
#define SOFT_SHADOWS 1
#define INTERPOLATE_SHADOWS 1

#include "LightBuffer.hlsli"

float LoadShadowMap(int2 pos)
{
    return shadowAtlas.Load(int3(pos, 0)).x;
}

float SampleShadowMap(float2 pixelCoords, float bias)
{
    int2 fpos = floor(pixelCoords);
    int2 v = int2(0, 1);
    float2x2 samples;
    samples._11 = LoadShadowMap(fpos) + bias;
    samples._12 = LoadShadowMap(fpos + v.yx) + bias;
    samples._21 = LoadShadowMap(fpos + v.xy) + bias;
    samples._22 = LoadShadowMap(fpos + v.yy) + bias;
    
    samples = saturate((-samples) * 100);
    float2 t = pixelCoords - fpos;
    t = .5 - cos(t * 3.141592) * .5;
    return (samples._11 * (1 - t.x) + samples._12 * t.x) * (1 - t.y) + (samples._21 * (1 - t.x) + samples._22 * t.x) * t.y;
}

float3 GetLight(float4 lPos)
{
    float shadow = 0.f;
    
    lPos.xyz /= lPos.w;
    
    shadow = 1 - saturate(max(abs(lPos.x), abs(lPos.y))*4-3);
    
    lPos.x = lPos.x / 2 + 0.5;
    lPos.y = lPos.y / -2 + 0.5;
    float2 pixelCoords = lPos.xy * SHADOWMAP_SIZE;
    
    float fac = 0;
    
    float bias = .005f;
    
    #if SOFT_SHADOWS
    
    int nSamples = 4;
    float e = .5f;
    
    [unroll(9)]
    for (int i = -nSamples; i <= nSamples; i++)
    {
        [unroll(9)]
        for (int j = -nSamples; j <= nSamples; j++)
        {
            #if INTERPOLATE_SHADOWS
            fac += SampleShadowMap(pixelCoords + float2(i, j)*e, bias - lPos.z);
            #else
            fac += saturate((lPos.z-LoadShadowMap(int2(i, j) + pixelCoords) - bias)*100);
            #endif
        }
    }
    fac /= pow(nSamples * 2 + 1, 2);
    
    #else
    
    #if INTERPOLATE_SHADOWS
    fac += SampleShadowMap(pixelCoords, bias - lPos.z);
    #else
    fac += saturate((lPos.z-LoadShadowMap(pixelCoords) - bias)*100);
    #endif
    
    #endif
    
    shadow *= fac;
   
    return lerp(diff.xyz, amb.xyz, shadow);

}