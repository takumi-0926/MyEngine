#include "FBX.hlsli"

struct SkinOutput {
	float4 pos;
	float3 normal;
};

//�X�L�j���O�v�Z
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

//�G���g���[�|�C���g
float4 shadowVS(VSInput input) : SV_POSITION
{
	//�X�L�j���O�v�Z
	SkinOutput skinned = ComputeSkin(input);

	//�s��ɂ����W�ϊ�
	skinned.pos = mul(world, skinned.pos);

	return mul(lightCamera, skinned.pos);
}