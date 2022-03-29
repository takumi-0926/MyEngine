
Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

////�萔�o�b�t�@
//cbuffer cbuff0 : register(b0) {
//	matrix world;
//	matrix viewproj;
//}
cbuffer cbuff0 : register(b0) {
	float4 color;
	matrix mat;
}

//�}�e���A���p
cbuffer Material : register(b1) {
	float4 diffuse;
	float4 specular;
	float3 ambient;
}

struct Output {
	float4 svpos : SV_POSITION;//�V�X�e���p���_���W
	float3 normal : NORMAL;//�@���x�N�g��
	float2 uv : TEXCOORD;//UV�l
};
