#include "hitBoxHeader.hlsli"

VSOutput main(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneno : BONE_NO, min16uint weight : WEIGHT)
{
	VSOutput output;
	float w = weight / 100.0f;
	matrix bm = bones[boneno[0]] * w + bones[boneno[1]] * (1 - w);

	pos = mul(bm, pos);
	output.svpos = mul(mul(viewproj, world), pos);
	normal.w = 0;
	output.normal = mul(world, normalize(normal));
	output.uv = uv;
	return  output;
}