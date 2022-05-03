#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include "includes.h"

class PMDmodel {
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//using TexMetadata = DirectX::TexMetadata;
	//using ScratchImage = DirectX::ScratchImage;

	//std::�ȗ�
	using string = std::string;

private:
	//�V�F�[�_�ɓ�������}�e���A���f�[�^
	struct MaterialForHlsl {
		XMFLOAT3 diffuse;//�f�B�t���[�Y�F
		float alpha;	//�f�B�t���[�Y��
		XMFLOAT3 specular;//�X�y�L�����F
		float specularStrength;//�X�y�L�����̋����i��Z�l�j
		XMFLOAT3 ambient;//�A���r�G���g�F
	};

	//��L�ȊO�̃}�e���A���f�[�^
	struct AdditionalMaterial {
		string texpath;
		int toonIdx;
		bool edgeFlg;
	};

	//�S�̂��܂Ƃ߂�f�[�^
	struct Material {
		unsigned int indicesNum;
		MaterialForHlsl material;
		AdditionalMaterial additional;
	};
	Material material;

	struct Transform {
		void* operator new(size_t size);
		XMMATRIX world;
	};

private:
	// �f�o�C�X
	static ID3D12Device* device;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;
	// �f�X�N���v�^�q�[�v
	static ComPtr<ID3D12DescriptorHeap> descHeap;

	//// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	//CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	//// �V�F�[�_���\�[�X�r���[�̃n���h��(GPU)
	//CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;


	// ���_�o�b�t�@,�r���[
	ComPtr<ID3D12Resource> vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// �C���f�b�N�X�o�b�t�@,�r���[
	ComPtr<ID3D12Resource> indexBuff;
	D3D12_INDEX_BUFFER_VIEW ibView;

	//���W�ϊ�
	ComPtr<ID3D12Resource> transformMat = nullptr;
	ComPtr<ID3D12DescriptorHeap> transformHeap = nullptr;

	//�q�[�v�̈�
	static ComPtr<ID3D12DescriptorHeap> materialDescHeap;

	//�}�e���A��
	uint32_t materialNum;
	std::vector<Material>materials;
	ComPtr<ID3D12Resource> materialBuff;
	std::vector<ComPtr<ID3D12Resource>> textureResources;
	std::vector<ComPtr<ID3D12Resource>> sphResources;
	std::vector<ComPtr<ID3D12Resource>> spaResources;
	std::vector<ComPtr<ID3D12Resource>> toonResources;

private:
	//���f���̃p�X�ƃe�N�X�`���̃p�X���獇���p�X�𓾂�
//@param modelpath �A�v���P�[�V�������猩��pmd���f���̃p�X
//@param texPath PMD ���f�����猩���e�N�X�`���̃p�X
//@return �A�v���P�[�V�������猩���e�N�X�`���̃p�X
	static string GetTexturePathFromModelAndTexPath(const string& modelPath, const char* texPath) {
		int pathIndex1 = modelPath.rfind('/');
		int pathIndex2 = modelPath.rfind('\\');

		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}

	//std::string{�}���`�o�C�g������}����std::wstring{���C�h������}�𓾂�
	//@param str �}���`�o�C�g������
	//@return �ϊ����ꂽ������
	static std::wstring GetWideStringFromString(const string& str)
	{
		//�Ăяo��1���(�����񐔂𓾂�)
		auto num1 = MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1, nullptr, 0);

		std::wstring wstr;//string��wchar_t��
		wstr.resize(num1);//����ꂽ�����񐔂Ń��T�C�Y

		//�Ăяo��2���(�m�ۍς݂�wstr�ɕϊ���������R�s�[)
		auto num2 = MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1, &wstr[0], num1);

		assert(num1 == num2);//�`�F�b�N
		return  wstr;
	}


	//�t�@�C��������g���q���擾����
	//@param path �Ώۂ̃p�X������
	//@return �g���q
	static string GetExtension(const string& path) {
		int idx = path.rfind('.');
		return path.substr(idx + 1, path.length() - idx - 1);
	}

	//�e�N�X�`���̃p�X���Z�p���[�^�[�����Ŏ擾����
	//@param path �Ώۂ̃p�X������
	//@param splitter ��؂蕶��
	//@return �����O��̕�����y�A
	static std::pair<string, string>SplitFileName(const string& path, const char splitter = '*') {
		int idx = path.find(splitter);
		std::pair<string, string>ret;
		ret.first = path.substr(0, idx);
		ret.second = path.substr((idx + 1), path.length() - idx - 1);

		return ret;
	}

	ComPtr<ID3D12Resource> LoadTextureFromFile(string& texPath);

	ID3D12Resource* CreateWhiteTexture();
	ID3D12Resource* CreateBlackTexture();

public:
	PMDmodel();

	static PMDmodel* Create();

	//�ÓI������
	static bool StaticInitialize(ID3D12Device* device);
	//�q�[�v������
	static bool InitializeDescriptorHeap();

	//���f���ǂݍ���
	void CreateModel(const std::string& strModelPath);

	//�e�N�X�`���ǂݍ���
	void Loadtexture();

	//�}�e���A���ǂݍ���
	void LoadMaterial(const std::string& strModelPath);


public:

	bool Initialize();

	void Update();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	//�A�N�Z�b�T
	ID3D12DescriptorHeap* MaterialDescHeap() { return materialDescHeap.Get(); }
	ID3D12DescriptorHeap* DescHeap() { return descHeap.Get(); }
	D3D12_VERTEX_BUFFER_VIEW VbView() { return vbView; }
	D3D12_INDEX_BUFFER_VIEW IbView() { return ibView; }
	std::vector<Material> Materials() { return materials; }

public://�����o�ϐ�
	ComPtr<ID3D12Resource> PMDconstBuffB1; // �萔�o�b�t�@
};