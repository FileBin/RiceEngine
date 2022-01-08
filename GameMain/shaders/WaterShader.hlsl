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

float3 palette(float val, float p, float smoothing, float3 col) {
    float3 col1 = lerp(col, .8, clamp((val + .005 - p) / (smoothing + .000001) + .1, 0, 1));
    return lerp(col1, 1., clamp((val - p) / (smoothing + .000001) + .1, 0, 1));
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    float2 res = Resolution.xy; //float2(800, 600);
    float3 n = input.norm;
    
    float2 texcoord = input.world_pos.xz * .25;
    float3 nn = norm(texcoord);
    
   // return float4(nn.xzy * .5 + .5, 1);
    
    float3 nx =-nn.x * input.xAxis * .3;
    float3 nz =-nn.z * input.zAxis * .3;
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
   
    float noise = waterN(texcoord);
   // float Nmask = simplexNoise(texcoord * .005);
    depth = 1 - input.pos.z;
    depth *= (input.pos.w * input.pos.w);
    float val = dot(nn, float3(0, 1, 0));
    float3 wcol = col;
    float k = .997; //+ Nmask * .01;
    
    float antiAliasF = dot(n, float3(0, 1, 0));
    antiAliasF = pow(antiAliasF, 10.);
    antiAliasF = clamp(antiAliasF, .25, 1);
    float smoothF = clamp(clamp(depth, 0, antiAliasF + .5) +  antiAliasF * depth * .2 - .2, .0005, 1.);
    //return float4(smoothF.xxx, 1);
    
    //Waves
    col = palette(val,
    k, 
    clamp(smoothF * .2 - .01, .001, .2),
    col);
    float alpha = 1. - exp(-depthval * 10.);
    
    //Borders
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
    col += s;
    return float4(col, alpha);
}