cbuffer cbuff1 : register(b1) {
	uint shift;
	uint mosaic;
}

struct VSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD; 
};