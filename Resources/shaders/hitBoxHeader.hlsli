//定数バッファ
cbuffer cbuff0 : register(b0) {
	matrix viewproj;  //ビュープロジェクション
	matrix world;
	float3 cameraPos; // カメラ座標（ワールド座標）
	matrix bones[256];//ボーン行列
}

cbuffer cbuff1 : register(b1) {
	float3 m_ambient : packoffset(c0);
	float3 m_diffuse : packoffset(c1);
	float3 m_specular : packoffset(c2);
	float m_alpha : packoffset(c2.w);
}

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	float4 worldpos : POSITION; // ワールド座標
	float3 normal :NORMAL; // 法線
	float2 uv  :TEXCOORD; // uv値
};
