#define SAMPLES 4

const Texture2DMS<float4, SAMPLES> renderTarget : register(t0);
SamplerState Samp : register(s0);
Texture2DMS<float, SAMPLES> depthBuffer : register(t1);

#define NEAR .01
#define FAR 1000
#define FOCUS 20.f
#define MAX_SAMPLES 4.f

float4 SampleRenderTarget(int2 pixelCoords)
{
    float4 col = 0.0f;
    for (uint sample = 0; sample < SAMPLES; ++sample)
    {
        col += renderTarget.Load(pixelCoords, sample);
    }
    col /= float(SAMPLES);
    return col;
}

float GetDepth(int2 pixelCoords)
{
    float z_b = depthBuffer.Load(pixelCoords, 0);
    float z_n = 2.0 * z_b - 1.0;
    float z_e = 2.0 * NEAR * FAR / (FAR + NEAR - z_n * (FAR - NEAR));
    return z_e;
}


float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    const float3 fogColor = float3(1, 1, 1);
    
    int2 pixelCoords = pos.xy;
    float numSamples = 0;
    float4 col = 0;
    //float4 col = SampleRenderTarget(pixelCoords);
    float focusFact = GetDepth(pixelCoords) - FOCUS;
    if(focusFact>0)
        focusFact /= FAR - FOCUS;
    else
        focusFact = - focusFact / FOCUS;
    focusFact = exp(focusFact);
    focusFact -= .5;
    //focusFact *= .1;
    //focusFact -= 5;
    focusFact = clamp(focusFact, 0.f, MAX_SAMPLES);
    for (int i = -focusFact; i <= focusFact; i++)
    {
        for (int j = -focusFact; j <= focusFact; j++)
        {
            col += SampleRenderTarget(pixelCoords + int2(i, j));
            numSamples++;
        }
    }
    
    col.xyz /= numSamples;

    col.xyz = pow(col.xyz, 2.);
    
    
    
    col.a = 1;
    return col;
}