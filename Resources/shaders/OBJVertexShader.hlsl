#include "OBJShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//VSOutput output; // ピクセルシェーダーに渡す値
	//output.svpos = mul(mat, pos);
	//output.normal = normal;
	//output.uv = uv;

	// 法線にワールド行列によるスケーリング・回転を適用
	float4 wnormal = normalize(mul(world, float4(normal, 0)));
	float4 wpos = mul(world, pos);

	VSOutput output; // ピクセルシェーダーに渡す値
	output.svpos = mul(mul(viewproj, world), pos);
	//output.svpos = mul(mul(lightCamera, world), pos);

	output.worldpos = mul(world, pos);
	output.normal = wnormal.xyz;
	output.uv = uv;
	pos = mul(world, pos);
	output.tpos = mul(lightCamera, pos);
	return output;
}