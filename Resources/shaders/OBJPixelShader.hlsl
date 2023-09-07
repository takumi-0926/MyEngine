#include "OBJShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー
Texture2D<float> lightDepthTex : register(t1);

struct PSOutput {
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
	float4 target2 : SV_TARGET2;
	float4 target3 : SV_TARGET3;
	float4 target4 : SV_TARGET4;
	float4 target5 : SV_TARGET5;
	float4 target6 : SV_TARGET6;
};

float4 main(VSOutput input) : SV_TARGET
{
	PSOutput output;

	float offset = 1.0f;
	float3 light = normalize(float3(1, -1, 1));
	float3 brightness = dot(-light, normalize(input.normal.xyz));
	//テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);

	//光沢度
	const float shininess = 4.0f;

	//環境反射光
	float3 ambient = m_ambient;
	//シェーディングによる色
	float4 shadecolor = float4(lightcolor * ambient, m_alpha);

	//頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	//シャドウマップ
	float shadowWeight = 1.0f;
	//範囲を0～1に
	float posFromLightUV = 1.0f / input.tpos.w;
	float2 shadowUV;
	shadowUV.x = 0.5f + (input.tpos.x / input.tpos.w) * 0.5f;
	shadowUV.y = 1.0f - (input.tpos.y / input.tpos.w) * 0.5f;

	float depthFromLight = lightDepthTex.Sample(smp, shadowUV).x;

	//深度値を比較
	if (shadowUV.x >= 0 && shadowUV.x <= 1.0f && shadowUV.y >= 0 && shadowUV.y <= 1.0f) {
		if (depthFromLight + 0.005f < input.tpos.z / input.tpos.w) {
			//shadecolor.xyz = shadecolor.xyz * 0.5f;
			shadowWeight = 0.5f;
		}
	}

	//平行光源
	for (int i = 0; i < DIRLIGHT_NUM; i++) {
		if (dirLights[i].active) {
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// 全て加算する
			shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
			shadecolor.a = m_alpha;
		}
	}

	// 点光源
	for (i = 0; i < POINTLIGHT_NUM; i++) {
		if (pointLights[i].active) {
			// ライトへの方向ベクトル
			float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			float d = length(lightv);
			lightv = normalize(lightv);

			// 距離減衰係数
			float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d);

			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// 全て加算する
			shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor;
		}
	}

	//丸影
	for (i = 0; i < CIRCLESHADOW_NUM; i++)
	{
		if (circleShadows[i].active) {
			float3 casterV = circleShadows[i].casterPos - input.worldpos.xyz;
			float d = dot(casterV, circleShadows[i].dir);
			float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
			atten *= step(0, d);

			float3 lightPos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;
			float3 lightV = normalize(lightPos - input.worldpos.xyz);
			float cos = dot(lightV, circleShadows[i].dir);
			float angleAtten = smoothstep(circleShadows[i].factorAngleCos.y, circleShadows[i].factorAngleCos.x, cos);
			atten *= angleAtten;

			shadecolor.rgb -= atten;
		}
	}

	float p = dot(input.normal * -1.0f, eyedir.xyz);
	p = p * 0.5f + 0.5f;
	p = p * p;
	float4 toonCol = tex.Sample(smp, float2(2.0f, 0.0f));

	float _ThresHoldA = 0.5f;
	float _ThresHoldB = 0.6f;
	float4 lightColor = float4(texcolor.rgb, 1);
	float4 darkColor = float4(texcolor.rgb, 1) * 0.2;

	float intensity = saturate(dot(normalize(input.normal), eyedir));

	//トゥーン
	float4 toon = smoothstep(_ThresHoldA, _ThresHoldB, intensity) * lightColor
		+ (1 - smoothstep(_ThresHoldA, _ThresHoldB, intensity)) * darkColor;

	//ネガポジ反転
	float4 NegColor = float4(1 - texcolor.r, 1 - texcolor.g, 1 - texcolor.b, 1);

	//モザイク
	float4 density = 50;
	float4 MozColor = tex.Sample(smp, floor(input.uv * density) / density);

	//RGBシフト
	float shift = 0.05;
	float r = tex.Sample(smp, input.uv + float2(-shift, 0)).r;
	float g = tex.Sample(smp, input.uv + float2(0, 0)).g;
	float b = tex.Sample(smp, input.uv + float2(shift, 0)).b;
	float4 ShifColor = float4(r, g, b, 1);

	//ディゾルブ
	float Dissolve = 0.9;
	float4 DissoColor = tex.Sample(smp, input.uv);
	DissoColor.a = step(DissoColor.r, Dissolve);

	//シェーディングによる色で描画
	output.target0 =
		float4((shadecolor.xyz * texcolor.xyz * shadowWeight),
			(shadecolor.w * texcolor.w));//通常
	output.target1 =
		float4((toon.xyz * texcolor.xyz * shadowWeight),
			(toon.w * texcolor.w));//トゥーン
	output.target2 =
		float4((NegColor.xyz * shadowWeight),
			NegColor.w);//ネガポジ
	output.target3 =
		float4((shadecolor.xyz * MozColor.xyz * shadowWeight),
			shadecolor.w * MozColor.w);//モザイク
	output.target4 =
		float4((ShifColor.xyz * shadowWeight),
			ShifColor.w);//RGBシフト
	output.target5 =
		float4((DissoColor.xyz * shadowWeight),
			DissoColor.w);//ディゾルブ
	return output.target3;
	//return shadecolor * texcolor/* * toonCol*/;
	//return outputfogcolor * texcolor/* * toonCol*/;
	//return float4((shadecolor.xyz * texcolor.xyz * shadowWeight),(shadecolor.w * texcolor.w)) /* * toonCol*/;

}