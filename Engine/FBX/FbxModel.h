#pragma once

#include <string>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

#include "fbxsdk.h"

struct Node {
	std::string name;
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	DirectX::XMMATRIX transform;
	DirectX::XMMATRIX globleTransForm;
	Node* parent = nullptr;
};

class FbxModel {
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	using string = std::string;
	template <class T> using vector = std::vector<T>;

private:
	ComPtr<ID3D12Resource> vertBuff;
	ComPtr<ID3D12Resource> indexBuff;
	ComPtr<ID3D12Resource> texBuff;

	D3D12_VERTEX_BUFFER_VIEW vbview = {};
	D3D12_INDEX_BUFFER_VIEW ibview = {};

	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

	FbxScene* fbxScene = nullptr;


public:
	~FbxModel();
	void CreateBuffers(ID3D12Device* device);

	void Update();

	void Draw(ID3D12GraphicsCommandList* cmdList);

public:
	friend class FbxLoader;

	static const int MAX_BONE_INDICES = 4;

	// �萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient; // �A���r�G���g�W��
		float pad1; // �p�f�B���O
		XMFLOAT3 diffuse; // �f�B�t���[�Y�W��
		float pad2; // �p�f�B���O
		XMFLOAT3 specular; // �X�y�L�����[�W��
		float alpha;	// �A���t�@
	};

	struct VertexPosNormalUvSkin {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		UINT boneIndex[MAX_BONE_INDICES];//�{�[���ԍ�
		float boneWeight[MAX_BONE_INDICES];//�{�[���̏d��
	};

	//�{�[���\����
	struct Bone {
		string name;//���O
		XMMATRIX invInitialPose;//�����p���̋t�s��
		FbxCluster* fbxCluster;//�N���X�^�[(FBX���̃{�[�����)
		Bone(const string& name) {//�R���X�g���N�^
			this->name = name;
		}
	};

	DirectX::XMFLOAT3 ambient = { 1,1,1 };

	DirectX::XMFLOAT3 diffuse = { 1,1,1 };

	float alpha = 1.0f;		// �A���t�@

	DirectX::TexMetadata metadata = {};

	DirectX::ScratchImage scrachImg = {};

	Node* meshNode = nullptr;

	std::vector<VertexPosNormalUvSkin> vertices;

	std::vector<unsigned short> indices;

	//���\�[�X�ԍ�
	int TextureOffset = 0;

	//�{�[���z��
	vector<Bone> bones;
private:
	std::string name;

	std::vector<Node> nodes;

	// �f�o�C�X
	static ID3D12Device* device;

	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;

public:
	//�Q�b�^�[
	const XMMATRIX& GetModelTransform() { return meshNode->globleTransForm; }
	vector<Bone>& GetBones() { return bones; }
	FbxScene* GetFbxScene() { return fbxScene; }
	int GetTextureOffset() { return TextureOffset; }

	void SetTextureOffset(int num) { this->TextureOffset = num; }

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	void CreateConstantBuffer();

};