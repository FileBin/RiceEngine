cbuffer ConstantBuffer {
	float4x4 World;
    float4x4 View;
	float4x4 Projection;
}

struct PixelShaderInput {
	float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float3 worldPos : POSITION0;
    float3 camPos: POSITION1;
    /*float4 tangent : TANGENT;
    float4 texCoord : TEXCOORD;*/
	//float4 color: COLOR0;
};

PixelShaderInput main(float3 pos : POSITION, float4 norm : NORMAL) {
    PixelShaderInput output = (PixelShaderInput)0;

    output.camPos = float3(View._14, View._24, View._34);
    float4x4 v = View;
    v._14 = 0;
    v._24 = 0;
    v._34 = 0;
    output.pos = float4(pos.xyz, 1.0f);
    output.pos = mul(output.pos, World);
    output.pos = mul(output.pos, v);
    output.pos = mul(output.pos, Projection);
    output.norm = norm;
    output.worldPos = mul(pos, World);
    return output;
}