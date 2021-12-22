cbuffer ConstantBuffer {
	float4x4 World;
    float4x4 View;
	float4x4 Projection;
}

struct PixelShaderInput {
	float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float4 viewPos : POSITION0;
    float3 light : POSITION2;
    /*float4 tangent : TANGENT;
    float4 texCoord : TEXCOORD;*/
	//float4 color: COLOR0;
};

PixelShaderInput main(float3 pos : POSITION, float4 norm : NORMAL) {
    PixelShaderInput output = (PixelShaderInput)0;

    float3 light = float3(2, -5, -1);
    light = normalize(light);

    float4x4 v = View;
    v._14 = 0;
    v._24 = 0;
    v._34 = 0;

    output.pos = float4(pos.xyz, 1.0f);
    output.pos = mul(output.pos, World);
    output.pos = mul(output.pos, v);
    output.pos = mul(output.pos, Projection);

    output.norm = mul(norm.xyz, World);
    output.norm = mul(output.norm, v);

    output.light = mul(light, World);
    output.light = mul(output.light, v);
    
    output.viewPos = float4(pos.xyz, 1.0f);
    output.viewPos = mul(output.viewPos, World);
    output.viewPos = mul(output.viewPos, v);

    return output;
}