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

Texture2D tex;
SamplerState samp;

cbuffer CBuffer {
    float time;
    float4 Resolution;
}

float _rand(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

float rand(float2 uv)
{
    uv %= 256;
    float t = max(uv.x, uv.y);
    t = lerp(t, min(uv.x, uv.y), .5);
    return lerp(_rand(uv), _rand(uv + 256), .5);
}

float noise(float2 pos)
{
    float2 fpos = floor(pos);
    float2 v = float2(0, 1);
    float2x2 samples;
    samples._11 = rand(fpos);
    samples._12 = rand(fpos + v.yx);
    samples._21 = rand(fpos + v.xy);
    samples._22 = rand(fpos + v.yy);
    float2 t = pos - fpos;
    t = .5 - cos(t*3.141592) * .5;
    return (samples._11 * (1 - t.x) + samples._12 * t.x) * (1 - t.y) + (samples._21 * (1 - t.x) + samples._22 * t.x) * t.y;
}

float simplexNoise(float2 uv)
{
    float val = 0;
    val += noise(uv) * .75;
    val += noise(uv * 2) * .25;
    val += noise(uv * 4) * .125;
    val += noise(uv * 8) * .0625;
    val *= .8;
    return val;
}

float waterN(float2 texcoord)
{
    float n1 = simplexNoise(texcoord + time * .2);
    float n2 = simplexNoise(texcoord - time * .2);
    float offset = lerp(n1, n2, .5);
    float noise = simplexNoise(texcoord + offset * 5) * simplexNoise(texcoord);
    return noise;
}

float3 norm(float2 pos)
{
    float2 e = float2(.05, 0);
    float d = waterN(pos);
    return normalize(float3(d - waterN(pos + e.xy), e.x, d - waterN(pos + e.yx)));

}

float4 main(PixelShaderInput input) : SV_TARGET
{
    float2 res = Resolution.xy; //float2(800, 600);
    float3 n = input.norm;
    
    float2 texcoord = input.world_pos.xz * .7;
    float3 nn = norm(texcoord);
    
    float3 nx = nn.x * input.xAxis * .3;
    float3 nz = nn.z * input.zAxis * .3;
    float3 ny = nn.y * input.norm;
    
    n = normalize(nx + ny + nz);
    
    int3 loc = int3(input.pos.xy, 0);
    float depth = (tex.Load(loc));
    float pixelDepth = input.pos.z;
    if (pixelDepth > depth)
        return 0;
    pixelDepth = depth - pixelDepth;
    
    float depthval = pixelDepth * 10 * (input.pos.w * input.pos.w + depth * depth);
    
    float emission = .2;
    float glossines = 32;
    float specular = 1.;
    float3 col = lerp(float3(.1, .15, .5), float3(.25, .55, .77), exp(-depthval));
   
    float3 eye = normalize(input.viewPos.xyz);

    float3 light = input.light;

    float em = emission;
	
    float spec = max(dot(reflect(light, n), -eye), 0);
    spec = pow(spec, glossines + 1);
    float3 s = 1 - col;
    s *= spec;
    s *= specular;
    if (length(s) > .5)
        s = 1;
    else
        s = 0;
    col += s;
   
    float noise = waterN(texcoord);
    depth = 1 - input.pos.z;
    depth *= (input.pos.w * input.pos.w);
    float val = dot(nn, float3(0, 1, 0));
    float3 wcol = col;
    float k = .89;
    if (val < k)
        col = 1.;
    else if (val < k + .05)
        col = lerp(col,1.,.8);
    col = lerp(col, wcol, clamp(depth, 0, 1));
    float alpha = 1. - exp(-depthval * 10.);
    if (alpha - noise < .2)
    {
        col = 1.;
        alpha = 1;
    }
    else if (alpha - noise < .3)
    {
        col = lerp(wcol, 1., .8);
        alpha = .8;
    }
    return float4(col, alpha);
}