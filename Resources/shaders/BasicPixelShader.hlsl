#include "BasicShaderHeader.hlsli"

float4 BasicPS(Output input) : SV_TARGET
{
	float3 light = normalize(float3(1, -1, 1));
	float3 brightness = dot(-light, normalize(input.normal.xyz));
	float2 normalUV = (input.normal.xy + float2(1, -1)) * float2(0.5f, -0.5f);
	//return float4(input.uv,0,1);
	//return float4(tex.Sample(smp,input.uv));
	//return float4(0,0,0,1);
	//return float4(input.normal.xyz,1);

	//return float4(1, brightness, brightness, 1);

	float shadowWeight = 1.0f;
	float3 posFromLightVP = input.tpos.xyz / input.tpos.w;
	//float2 shadowUV = (posFromLightVP + float2(1, -1)) * float2(0.5f, -0.5f);

	//float depthFromLight = lightDepthTex.Sample(smp, shadowUV);

	//if (depthFromLight < posFromLightVP.z) {
	//	shadowWeight = 0.5f;
	//}

	if (input.instNo == 1) {
		return float4(0.2f, 0.2f, 0.2f, 1);
	}

	//float b = brightness * shadowWeight;

	return float4(brightness,1)
		* diffuse
		* tex.Sample(smp,input.uv)
		* sph.Sample(smp,normalUV)
		+ spa.Sample(smp,normalUV);
}