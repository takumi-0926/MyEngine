#include "Basic.hlsli"

Output main(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneno : BONE_NO, min16uint weight : WEIGHT) {
	Output output;
	output.svpos = mul(mat,pos);
	output.normal = normal;
	//output.svpos = mul(mul(viewproj, world), pos);
	//normal.w = 0;
	//output.normal = mul(world, normal);
	//output.svpos = pos;
	output.uv = uv;
	return  output;
	//return pos + float4(0.5,0.5,0,0);//オフセット
		//return pos * float4(0.5,0.5,1,1);//倍率
}