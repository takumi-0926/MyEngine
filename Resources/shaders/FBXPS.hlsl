#include "FBX.hlsli"

//
Texture2D<float4> tex : register(t0);

//Texture2D<float> lightDepthTex : register(t1);

//
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	//
	float4 texcolor = tex.Sample(smp,input.uv);

	//環境反射光
	float3 ambient = m_ambient;

	//
	float3 light = normalize(float3(1, -1, 1));
	float diffuse = saturate(dot(-light, input.normal));
	float brightness = diffuse + 0.3f;
	float4 shadercolor = float4(ambient, m_alpha);
	shadercolor.rgb += brightness;


	////シャドウマップ
	//float shadowWeight = 1.0f;
	////範囲を0～1に
	//float3 posFromLightUV = input.tpos.xyz / input.tpos.w;
	//float2 shadowUV = (posFromLightUV.xy + float2(1, -1)) * float2(0.5f, -0.5f);

	//float depthFromLight = lightDepthTex.Sample(smp, shadowUV);

	////深度値を比較
	//if (depthFromLight < posFromLightUV.z) {
	//	shadowWeight = 0.5f;
	//}

	//float4 shadercolor = float4(brightness, brightness, brightness, 1.0f);

	//
	return shadercolor * texcolor;
}