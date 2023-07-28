#include "ObjShaderHeader.hlsli"

float4 shadowVS(
	float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD) : SV_POSITION
{
	//// �@���Ƀ��[���h�s��ɂ��X�P�[�����O�E��]��K�p
	//float4 wnormal = normalize(mul(world, float4(normal, 0)));
	float4 wpos = mul(world, pos);
	//
	//VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
	//output.svpos = mul(mul(viewproj, world), pos);
	////output.svpos = mul(mul(lightCamera, world), pos);
	//
	//output.normal = wnormal.xyz;
	//output.uv = uv;
	return mul(mul(lightCamera,world), pos);
}