#define SAMPLES 4

const Texture2DMS<float4, SAMPLES> renderTarget : register(t0);
SamplerState Samp : register(s0);

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

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    int2 pixelCoords = pos.xy;
    float4 col = SampleRenderTarget(pixelCoords);
    col.xyz = pow(col.xyz, 2.);
    col.a = 1;
	return col;
}