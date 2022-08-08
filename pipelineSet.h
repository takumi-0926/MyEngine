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

//�p�C�v���C���\����
struct PipelineSet {
	ComPtr<ID3D12PipelineState> _pipelinestate;
	ComPtr<ID3D12RootSignature> _rootsignature;
};

class LoadHlsls {
private:

public:
	static ComPtr<ID3DBlob> _errorBlob; // �G���[�I�u�W�F�N�g

	static std::map<int,PipelineSet> pipeline;
	static std::map<int,ComPtr<ID3DBlob>> vsBlob; //���_�V�F�[�_�i�[�p
	static std::map<int,ComPtr<ID3DBlob>> psBlob; //�s�N�Z���V�F�[�_�i�[�p
	
	//hlsl�t�@�C���̃��[�h�i���_�V�F�[�_�j
	static void LoadHlsl_VS(
		int Number,
		const wchar_t* namePath,
		const char* entryPathName,
		const char* modelPathName);
	//hlsl�t�@�C���̃��[�h�i�s�N�Z���V�F�[�_�j
	static void LoadHlsl_PS(
		int Number,
		const wchar_t* namePath,
		const char* entryPathName,
		const char* modelPathName);

	//�p�C�v���C������
	static void createPipeline(
		ID3D12Device* _dev, int Number);
};