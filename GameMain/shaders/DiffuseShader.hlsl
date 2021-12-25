struct PixelShaderInput {
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
	float4 viewPos : POSITION0;
	float3 light : POSITION2;
	float4 color : COLOR0;
  /*float4 tangent : TANGENT;
	float4 texCoord : TEXCOORD;*/
	//float4 color: COLOR0;
};

static float emission = .3;
static float glossines = 2.;
static float specluar = .8;

float4 main(PixelShaderInput input) : SV_TARGET{
	float3 n = input.norm.xyz;

	float3 eye = normalize(input.viewPos);

	float3 light = input.light;

	float em = emission;

	float3 col = float3(.8,0.5,0.2);
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