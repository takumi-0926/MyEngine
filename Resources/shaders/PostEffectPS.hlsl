#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0);
Texture2D<float> depth : register(t1);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	float dep = pow(abs(depth.Sample(smp, input.uv)), 20);

	//return float4(dep, dep, dep, 1);

	float4 texcolor = tex.Sample(smp,input.uv);
	//return float4(float3(1, 1, 1) - texcolor.rgb, 1);
	return float4(texcolor.rgb * 2.0f,1);
}