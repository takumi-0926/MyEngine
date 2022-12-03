#pragma once
#include"includes.h"

enum ShaderNo{
	TWO,
	THREE,
	OBJ,
	FBX,
	PMD,
	SINGLE,
	PHONG,
	TOON,
	BLOOM,
	DEPTH,
	GAUSSU,
	MASK,
	DISSO,
	HIT,
};

//パイプライン構造体
struct PipelineSet {
	ComPtr<ID3D12PipelineState> _pipelinestate;
	ComPtr<ID3D12RootSignature> _rootsignature;
};

class LoadHlsls {
private:

public:
	static ComPtr<ID3DBlob> _errorBlob; // エラーオブジェクト

	static std::map<int,PipelineSet> pipeline;
	static std::map<int,ComPtr<ID3DBlob>> vsBlob; //頂点シェーダ格納用
	static std::map<int,ComPtr<ID3DBlob>> psBlob; //ピクセルシェーダ格納用
	
	//hlslファイルのロード（頂点シェーダ）
	static void LoadHlsl_VS(
		int Number,
		const wchar_t* namePath,
		const char* entryPathName,
		const char* modelPathName);
	//hlslファイルのロード（ピクセルシェーダ）
	static void LoadHlsl_PS(
		int Number,
		const wchar_t* namePath,
		const char* entryPathName,
		const char* modelPathName);

	//パイプライン生成
	static void createPipeline(
		ID3D12Device* _dev, int Number);
};