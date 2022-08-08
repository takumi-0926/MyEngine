//�萔�o�b�t�@
cbuffer cbuff0 : register(b0) {
	matrix viewproj;  //�r���[�v���W�F�N�V����
	matrix world;
	float3 cameraPos; // �J�������W�i���[���h���W�j
	matrix bones[256];//�{�[���s��
}

cbuffer cbuff1 : register(b1) {
	float3 m_ambient : packoffset(c0);
	float3 m_diffuse : packoffset(c1);
	float3 m_specular : packoffset(c2);
	float m_alpha : packoffset(c2.w);
}

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos : SV_POSITION; // �V�X�e���p���_���W
	float4 worldpos : POSITION; // ���[���h���W
	float3 normal :NORMAL; // �@��
	float2 uv  :TEXCOORD; // uv�l
};
