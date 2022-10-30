#pragma once

#include <string>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

#pragma push
#pragma warning(disable:26495)
#pragma warning(disable:26812)
#pragma warning(disable:26451)
#include "fbxsdk.h"
#pragma pop

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

	~FbxModel();

public:
	void CreateBuffers(ID3D12Device* device);

	void Draw(ID3D12GraphicsCommandList* cmdList);

public:
	friend class FbxLoader;

	static const int MAX_BONE_INDICES = 4;

	struct VertexPosNormalUvSkin {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		UINT boneIndex[MAX_BONE_INDICES];//ボーン番号
		float boneWeight[MAX_BONE_INDICES];//ボーンの重み
	};

	//ボーン構造体
	struct Bone {
		string name;//名前
		XMMATRIX invInitialPose;//初期姿勢の逆行列
		FbxCluster* fbxCluster;//クラスター(FBX側のボーン情報)
		Bone(const string& name) {//コンストラクタ
			this->name = name;
		}
	};

	DirectX::XMFLOAT3 ambient = { 1,1,1 };

	DirectX::XMFLOAT3 diffuse = { 1,1,1 };

	DirectX::TexMetadata metadata = {};

	DirectX::ScratchImage scrachImg = {};

	Node* meshNode = nullptr;

	std::vector<VertexPosNormalUvSkin> vertices;

	std::vector<unsigned short> indices;

	//ボーン配列
	vector<Bone> bones;
private:
	std::string name;

	std::vector<Node> nodes;

public:
	//ゲッター
	const XMMATRIX& GetModelTransform() { return meshNode->globleTransForm; }
	vector<Bone>& GetBones() { return bones; }
	FbxScene* GetFbxScene() { return fbxScene; }
};