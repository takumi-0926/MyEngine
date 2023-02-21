#include "BasicShaderHeader.hlsli"

Output BasicVS(
	float4 pos : POSITION,
	float4 normal : NORMAL,
	float2 uv : TEXCOORD,
	min16uint2 boneno : BONE_NO,
	min16uint weight : WEIGHT,
	uint instNo : SV_InstanceID)
{
	Output output;
	float w = weight / 100.0f;
	matrix bm = (bones[boneno[0]] * w + bones[boneno[1]] * (1 - w));

	output.svpos = mul(world, mul(bm, pos));
	if (instNo == 1) { output.svpos = mul(shadow, output.svpos); }
	output.svpos = mul(viewproj, output.svpos);
	normal.w = 0;
	output.normal = mul(world, normalize(normal));
	output.uv = uv;
	output.instNo = instNo;
	return  output;
}

float4 shadowVS(
	float4 pos : POSITION,
	float2 uv : TEXCOORD,
	min16uint2 boneno : BONE_NO,
	min16uint weight : WEIGHT) : SV_POSITION
{
	float fweight = float(weight) / 100.0f;
	matrix conBone = bones[boneno.x] * fweight + bones[boneno.y] * (1.0f - fweight));

	pos = mul(world, mul(conbone, pos));

	return mul(lightCamera, pos);

}