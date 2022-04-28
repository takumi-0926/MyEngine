
Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);
Texture2D<float4> sph : register(t1);
Texture2D<float4> spa : register(t2);

//定数バッファ
cbuffer cbuff0 : register(b0) {
	matrix viewproj;
	matrix world;
	float3 cameraPos; // カメラ座標（ワールド座標）
}

//マテリアル用
cbuffer Material : register(b1) {
	float4 diffuse;
	float4 specular;
	float3 ambient;
}

struct Output {
	float4 svpos : SV_POSITION;//システム用頂点座標
	float4 normal : NORMAL;//法線ベクトル
	float2 uv : TEXCOORD;//UV値
};

