#pragma once
#include"includes.h"
//パイプライン構造体
struct PipelineSet {
	ComPtr<ID3D12PipelineState> _pipelinestate;
	ComPtr<ID3D12RootSignature> _rootsignature;
};

class LoadHlsls {
private:

public:
	static ComPtr<ID3DBlob> _errorBlob; // エラーオブジェクト

	//hlslファイルのロード
	static void LoadHlsl(
		const char* namePath,
		ID3DBlob** objBlob,
		const char* entryPathName,
		const char* modelPathName);

	//パイプライン生成
	PipelineSet createPipeline(
		ID3D12Device* _dev);
};