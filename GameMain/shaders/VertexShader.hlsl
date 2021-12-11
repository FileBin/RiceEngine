cbuffer ConstantBuffer {
	matrix World;
	matrix View;
	matrix Projection;
}

struct PixelShaderInput {
	float4 pos : SV_POSITION;
	float4 color: COLOR0;
};

PixelShaderInput main( float4 pos : POSITION, float4 color : COLOR0)
{
   /* PixelShaderInput output = (PixelShaderInput)0;
    output.pos = mul(pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    output.color = color;*/
    PixelShaderInput output = (PixelShaderInput)0;
    output.pos = float4(pos.xyz, 1.0f);
    output.pos = mul(pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    output.color = color;
    return output;
}