cbuffer cbuff0 : register(b0)
{
	matrix viewproj; // �R�c�ϊ��s��
	matrix matBillboard;
};

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 pos : POSITION; // �V�X�e���p���_���W
	float scale : TEXCOORD;
};

struct GSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};