struct PixelShaderInput {
    float4 pos : SV_POSITION;
    float4 color: COLOR0;
};

cbuffer ConstantBuffer {
    float time;
}

float4 main(PixelShaderInput input) : SV_TARGET{
    //float3 light = float3(1,-3,-2);
    float4 color = input.color;
    color.xyz = sin((sin(time*2)+2)*color.xyz);
    return color;
}