#include "FBX.hlsli"

//
Texture2D<float4> tex : register(t0);

//
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	//
	float4 texcolor = tex.Sample(smp,input.uv);

	//ŠÂ‹«”½ŽËŒõ
	float3 ambient = m_ambient;

	//
	float3 light = normalize(float3(1, -1, 1));
	float diffuse = saturate(dot(-light, input.normal));
	float brightness = diffuse + 0.3f;
	float4 shadercolor = float4(ambient, m_alpha);
	shadercolor.rgb += brightness;

	//float4 shadercolor = float4(brightness, brightness, brightness, 1.0f);

	//
	return shadercolor * texcolor;
}