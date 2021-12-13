struct PixelShaderInput {
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
    float3 worldPos : POSITION0;
    float3 camPos: POSITION1;
  /*float4 tangent : TANGENT;
	float4 texCoord : TEXCOORD;*/
	//float4 color: COLOR0;
};

float4 main(PixelShaderInput input) : SV_TARGET{

    float emission = .3;
    float glossines = 8.;
    float specluar = .8;


	float3 n = input.norm.xyz;

    float3 eye = normalize(input.worldPos - input.camPos);

    float em = emission;

    float3 light = float3(2, -5, -1);
    light = normalize(light);

    float3 col = float3(1,0.3,0.2);
    float dif = dot(-light, n);
    float spec = max(dot(reflect(light, n), -eye), 0);
    float shadow = 1;//shadowSoft(p, -LIGHT_DIR, .0625, MAX_DIST, MAX_DIST*.1);
    dif *= .5;
    dif += .5;
    spec = pow(spec, glossines + 1);
    dif = max(dif, em);
    em = 1 - em;
    col *= ((dif * shadow - 1) * em + 1);
    float3 s = 1 - col;
    s *= spec;
    col += s * shadow * specluar;

	float4 color = float4(col, 1);
	return color;
}