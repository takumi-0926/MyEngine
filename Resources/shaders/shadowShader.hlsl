#include "BasicShaderHeader.hlsli"

float4 shadowVS(
	float4 pos : POSITION,
	float2 uv : TEXCOORD,
	min16uint2 boneno : BONE_NO,
	min16uint weight : WEIGHT) : SV_POSITION
{
	float fweight = float(weight) / 100.0f;
	matrix conBone = bones[boneno.x] * fweight + bones[boneno.y] * (1.0f - fweight);

	pos = mul(world, mul(conBone, pos));

	return mul(lightCamera, pos);
}