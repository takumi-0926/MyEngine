static const int MAX_BONES = 256;

cbuffer cbuff0 : register(b0) {
	matrix viewproj;
	matrix world;
	float3 cameraPos;
	matrix lightCamera;
}

cbuffer cbuff1 : register(b1) {
	float3 m_ambient : packoffset(c0);
	float3 m_diffuse : packoffset(c1);
	float3 m_specular : packoffset(c2);
	float m_alpha : packoffset(c2.w);
}

cbuffer skinning:register(b3) {
	matrix matSkinning[MAX_BONES];
}

struct VSInput {
	float4 pos	: POSITION;
	float3 normal : NORMAL;
	float2 uv	: TEXCOORD;
	uint4 boneIndices : BONEINDICES;
	float4 boneWeights : BONEWEIGHTS;
};

struct VSOutput {
	float4 svpos : SV_POSITION;
	float4 tpos : TPOS;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};