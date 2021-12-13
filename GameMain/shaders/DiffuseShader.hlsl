struct PixelShaderInput {
	float4 pos : SV_POSITION;
	/*float3 norm : NORMAL;
	float4 tangent : TANGENT;
	float4 texCoord : TEXCOORD;*/
	//float4 color: COLOR0;
};

float4 main(PixelShaderInput input) : SV_TARGET{
	float3 pos = input.pos.xyz;
	float4 col = float4(pos.xyz, 1);
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
}