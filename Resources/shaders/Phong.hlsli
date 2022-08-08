cbuffer cbuff0 : register(b0)
{
	float color;
	matrix viewproj; // �r���[�v���W�F�N�V�����s��
	matrix world; // ���[���h�s��
	float3 cameraPos; // �J�������W�i���[���h���W�j
};

cbuffer cbuff1 : register(b1) {
	float3 lightv;
	float3 lightcolor;
}

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos : SV_POSITION; // �V�X�e���p���_���W
	float4 worldpos : POSITION; // ���[���h���W
	float3 normal :NORMAL; // �@��
	float2 uv  :TEXCOORD; // uv�l
};
