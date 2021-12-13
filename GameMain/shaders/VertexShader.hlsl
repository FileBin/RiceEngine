cbuffer ConstantBuffer {
	matrix World;
	matrix View;
	matrix Projection;
}

struct PixelShaderInput {
	float4 pos : SV_POSITION;
    /*float3 norm : NORMAL;
    float4 tangent : TANGENT;
    float4 texCoord : TEXCOORD;*/
	//float4 color: COLOR0;
};

PixelShaderInput main(float3 pos : POSITION) {
    PixelShaderInput output = (PixelShaderInput)0;
    output.pos = float4(pos.xyz, 1.0f);
    output.pos = mul(output.pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    //output.color = color;
    return output;
}