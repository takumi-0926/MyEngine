#include "OBJShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー
Texture2D<float> lightDepthTex : register(t1);

float4 main(VSOutput input) : SV_TARGET
{
	float offset = 1.0f;
	float3 light = normalize(float3(1, -1, 1));
	float3 brightness = dot(-light, normalize(input.normal.xyz));
	//テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);

	//return float4(brightness.x + offset, brightness.y + offset, brightness.z + offset, 1) * texcolor;

	//float3 light = normalize(float3(1,-1,1)); // 右下奥　向きのライト
	//float light_diffuse = saturate(dot(-light, input.normal));
	//float3 shade_color;
	//shade_color = m_ambient;
	//shade_color += m_diffuse * light_diffuse;
	//float4 texcolor = tex.Sample(smp, input.uv);
	//return float4(texcolor.rgb, texcolor.a * m_alpha);
	//return float4(1, 1, 1, 1);


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
	float3 posFromLightUV = input.tpos.z / input.tpos.w;
	float2 shadowUV;
	shadowUV.x = (1.0f + posFromLightUV.x) * 0.5f;
	shadowUV.y = (1.0f - posFromLightUV.y) * 0.5f;

	float depthFromLight = lightDepthTex.Sample(smp, shadowUV).x;

	//深度値を比較
	if (depthFromLight - 0.005f < posFromLightUV.z ) {
		shadecolor.xyz *= 0.5f;
	}

	float2 uv = input.uv.xy;
	float fogWeight = 0.0f;

	float constant_fog_scale = 1.0f;
	float CONSTANT_FOG_ATTENUATION_RATE = 0.1f;

	const float3 sgColor = shadecolor.rgb;
	const float3 fogColor = 0.8f;

	fogWeight += constant_fog_scale * max(0.0f, 1.0f - exp(-CONSTANT_FOG_ATTENUATION_RATE * input.svpos.z));
	float4 outputfogcolor = float4(lerp(sgColor, fogColor, fogWeight),1);

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

	//シェーディングによる色で描画
	return outputfogcolor * texcolor/* * toonCol*/;
	return float4(shadecolor.xyz * texcolor.xyz * shadowWeight,shadecolor.w * texcolor.w) /* * toonCol*/;

}