#include "OBJShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//float3 light = normalize(float3(1,-1,1)); // 右下奥　向きのライト
	//float light_diffuse = saturate(dot(-light, input.normal));
	//float3 shade_color;
	//shade_color = m_ambient;
	//shade_color += m_diffuse * light_diffuse;
	//float4 texcolor = tex.Sample(smp, input.uv);
	//return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);
	//return float4(1, 1, 1, 1);

			//テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);

	//シェーディングによる色
	float4 shadecolor;

	//光沢度
	const float shininess = 4.0f;

	//頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	//ライトに向かうベクトルと法線の内積
	float3 dotlightnormal = dot(lightv, input.normal);

	//反射光ベクトル
	float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);

	//環境反射光
	float3 ambient = m_ambient;
	//拡散反射光
	float3 diffuse = dotlightnormal * m_diffuse;
	//鏡面反射光
	float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

	//すべて加算する
	shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
	shadecolor.a = m_alpha;

	float p = dot(input.normal * -1.0f, eyedir.xyz);
	p = p * 0.5f + 0.5f;
	p = p * p;
	float4 toonCol = tex.Sample(smp, float2(2.0f, 0.0f));

	//シェーディングによる色で描画
	return shadecolor * texcolor/* * toonCol*/;

}