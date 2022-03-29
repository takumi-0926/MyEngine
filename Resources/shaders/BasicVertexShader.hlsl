#include "BasicShaderHeader.hlsli"

Output BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneno : BONE_NO, min16uint weight : WEIGHT) {
	Output output;
	//output.svpos = mul(mat,pos);
	//output.normal = normal;
	//output.svpos = pos * float4(0.01,0.01,0.01,1);
	output.svpos = mul(mul(viewproj, world), pos);
	normal.w = 0;
	output.normal = mul(world, normal);
	//output.normal = normal;
	output.uv = uv;
	return  output;
}