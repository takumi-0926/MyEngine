#include "FBX.hlsli"

struct SkinOutput {
	float4 pos;
	float3 normal;
};

//スキニング計算
SkinOutput ComputeSkin(VSInput input) {
	SkinOutput output = (SkinOutput)0;

	uint iBone;
	float weight;
	matrix m;

	iBone = input.boneIndices.x;
	weight = input.boneWeights.x;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);

	iBone = input.boneIndices.y;
	weight = input.boneWeights.y;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);

	iBone = input.boneIndices.z;
	weight = input.boneWeights.z;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);

	iBone = input.boneIndices.w;
	weight = input.boneWeights.w;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);

	return output;
}

//エントリーポイント
float4 shadowVS(VSInput input) : SV_POSITION
{
	//スキニング計算
	SkinOutput skinned = ComputeSkin(input);

	//行列による座標変換
	skinned.pos = mul(world, skinned.pos);

	return mul(lightCamera, skinned.pos);
}