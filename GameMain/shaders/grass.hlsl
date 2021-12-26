cbuffer ConstantBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
}

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
	float4 viewPos : POSITION0;
	float3 light : POSITION2;
	float4 color : COLOR0;
    /*float4 tangent : TANGENT;
    float4 texCoord : TEXCOORD;*/
	//float4 color: COLOR0;
};

PixelShaderInput main(float3 pos : POSITION, float4 norm : NORMAL)
{
	PixelShaderInput output = (PixelShaderInput) 0;

	float3 light = float3(2, -5, -1);
	light = normalize(light);

	output.pos = float4(pos.xyz, 1.0f);
	output.pos = mul(output.pos, World);

	output.color = float4(0.2, 0.4, 0.2, 1);

	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);

	output.norm = mul(norm.xyz, World);
	output.norm = mul(output.norm, View);

	output.light = mul(light, World);
	output.light = mul(output.light, View);
    
	output.viewPos = float4(pos.xyz, 1.0f);
	output.viewPos = mul(output.viewPos, World);
	output.viewPos = mul(output.viewPos, View);

	return output;
}