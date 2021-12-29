struct PixelShaderInput {
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
	float4 viewPos : POSITION0;
	float3 light : POSITION2;
    float3 world_pos : POSITION3;
    float3 world_norm : POSITION4;
};

cbuffer CBuffer
{
    float time;
    float4 incolor;
    float3 egs;
}

float rand(float w_x, float w_y)
{
    float x = sin(dot(float2(w_x, w_y), float2(12.9898, 78.233)));
    return x - floor(x);
}

float clamp(float value, float min_, float max_)
{
    return min(max(value, min_), max_);
}

float4 main(PixelShaderInput input) : SV_TARGET{
    float3 n = input.norm;
    float3 w_n = normalize(input.world_norm);

	float emission = egs.x;
    float glossines = egs.y;
    float specular = egs.z;

    float add = 0;
    if (incolor.w < 1)
    {
        float com1 = clamp(w_n.z, 0, 1) * sin(time * rand(w_n.x, w_n.z));
        com1 = max(com1, 0);
        float com2 = clamp(w_n.x, 0, 1) * cos(time * rand(w_n.z, w_n.x));
        com2 = max(com2, 0);
        add = pow((com1 + com2) * 5, 0.5);
    }

    specular += add;

	float3 eye = normalize(input.viewPos);

	float3 light = input.light;

	float em = emission;
	
	float3 col = incolor.xyz;
	float dif = dot(-light, n);
	float spec = max(dot(reflect(light, n), -eye), 0);
	float shadow = 1;
	dif *= .5;
	dif += .5;
	spec = pow(spec, glossines + 1);
	dif = max(dif, em);
	em = 1 - em;
	col *= ((dif * shadow - 1) * em + 1);
	float3 s = 1 - col;
	s *= spec;
    col += s * shadow * specular;

    return float4(col, incolor.w);
}