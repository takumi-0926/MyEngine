#include "Basic.hlsli"

float4 main(Output input) : SV_TARGET
{
	float3 light = normalize(float3(1, -1, 1));
	float diffuse = saturate(dot(-light, input.normal));
	float brightness = diffuse + 0.3f;;
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	
	//return float4(input.uv,1,1);
	//return float4(tex.Sample(smp,input.uv));
	//return float4(0,0,0,1);
	//return float4(input.normal.xyz,1);
	//return float4(brightness, brightness, brightness, 1);
	//return float4(1.0f,1.0f,1.0f,1.0f);
	//return  float4(input.normal, 1);
	return float4(texcolor.rgb * brightness, texcolor.a) * color;

}