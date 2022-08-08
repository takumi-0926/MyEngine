#include "Phong.hlsli"

float4 main(VSOutput input) : SV_TARGET
{
	//float intensity =
	//saturate(dot(normalize(input.normal),_WorldSpaceLightPos0));

	//float reflection = pow(intensity, 20);

	//float3 eyedir = normalize(cameraPos.xyz - input.worldPos);

	//float4 specularColor = float4(1, 1, 1, 1);

	//float4 diffiseColor = float4(color.r, color.g, color.b, 1);

	//float4 ambient = float4(color.r, color.g, color.b, 1) * 0.5f;

	//float4 diffuse = intensity * diffiseColor;

	//float4 specular = reflection * specularColor;

	//float4 ads = ambient + diffuse + specular;
	float4 color = {1,1,1,1};
	return color;
}