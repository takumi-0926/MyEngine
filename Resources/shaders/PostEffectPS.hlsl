#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0);
Texture2D<float> depth : register(t1);
SamplerState smp : register(s0);

struct PSOutput {
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
	float4 target2 : SV_TARGET2;
	float4 target3 : SV_TARGET3;
	float4 target4 : SV_TARGET4;
	float4 target5 : SV_TARGET5;
	float4 target6 : SV_TARGET6;
};

float4 main(VSOutput input) : SV_TARGET
{
	PSOutput output;

	float dep = pow(depth.Sample(smp, input.uv), 20);

	float4 texcolor = tex.Sample(smp,input.uv);

	//モザイク
	float4 density = 50;
	float4 MozColor = tex.Sample(smp, floor(input.uv * density) / density);

	//RGBシフト
	float shift = 0.05;
	float r = tex.Sample(smp, input.uv + float2(-shift, 0)).r;
	float g = tex.Sample(smp, input.uv + float2(0, 0)).g;
	float b = tex.Sample(smp, input.uv + float2(shift, 0)).b;
	float4 ShifColor = float4(r, g, b, 1);

	output.target0 = MozColor;
	output.target1 = ShifColor;

	if (mosaic == 1) {
		return float4(output.target1.rgb, 1);
	}
	else if (shift == 1) {
		return float4(output.target1.rgb, 1);
	}
	else {
		return float4(texcolor.rgb,1);
	}
}