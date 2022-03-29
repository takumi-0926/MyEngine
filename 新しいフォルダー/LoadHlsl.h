#pragma once
#include"includes.h"
//�p�C�v���C���\����
struct PipelineSet {
	ComPtr<ID3D12PipelineState> _pipelinestate;
	ComPtr<ID3D12RootSignature> _rootsignature;
};

class LoadHlsls {
private:

public:
	ComPtr<ID3DBlob> _errorBlob; // �G���[�I�u�W�F�N�g

	//hlsl�t�@�C���̃��[�h
	void LoadHlsl(
		LPCUWSTR namePath,
		ID3DBlob** objBlob,
		const char* entryPathName,
		const char* modelPathName);

	//�p�C�v���C������
	PipelineSet createPipeline(
		ID3D12Device* _dev);
};