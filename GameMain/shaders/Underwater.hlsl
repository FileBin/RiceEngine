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
    
    for (int i = -4; i <= 4; i++)
    {
        for (int j = -4; j <= 4; j++)
        {
            float4 c = SampleRenderTarget(pixelCoords + int2(i, j));
            float depth = GetDepth(pixelCoords + int2(i, j));
    
            float3 waterColor = lerp(float3(.1, .15, .5), float3(.25, .55, .77), exp(-depth * .05));
    
            float fog = depth * .1;
    
            c.xy = pow(c.xy, 1.5);
    
            c.xyz = lerp(c.xyz, waterColor, clamp(fog, 0.f, 1.f));
            c.w = 1;
            col += c;
            numSamples++;
        }
    }
   
    
    col.xyz /= numSamples;

    col.xyz = pow(col.xyz, 2.);
    
    col.a = 1;
    return col;
}