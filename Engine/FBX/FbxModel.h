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

	// 定数バッファ用データ構造体B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient; // アンビエント係数
		float pad1; // パディング
		XMFLOAT3 diffuse; // ディフューズ係数
		float pad2; // パディング
		XMFLOAT3 specular; // スペキュラー係数
		float alpha;	// アルファ
	};

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

	float alpha = 1.0f;		// アルファ

	DirectX::TexMetadata metadata = {};

	DirectX::ScratchImage scrachImg = {};

	Node* meshNode = nullptr;

	std::vector<VertexPosNormalUvSkin> vertices;

	std::vector<unsigned short> indices;

	//リソース番号
	int TextureOffset = 0;

	//ボーン配列
	vector<Bone> bones;
private:
	std::string name;

	std::vector<Node> nodes;

	// デバイス
	static ID3D12Device* device;

	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;

public:
	//ゲッター
	const XMMATRIX& GetModelTransform() { return meshNode->globleTransForm; }
	vector<Bone>& GetBones() { return bones; }
	FbxScene* GetFbxScene() { return fbxScene; }
	int GetTextureOffset() { return TextureOffset; }

	void SetTextureOffset(int num) { this->TextureOffset = num; }

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	void CreateConstantBuffer();

};