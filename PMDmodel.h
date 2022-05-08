#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include "includes.h"

class PMDobject;
class PMDmodel {
	friend PMDobject;
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//using TexMetadata = DirectX::TexMetadata;
	//using ScratchImage = DirectX::ScratchImage;

	//std::省略
	using string = std::string;

	PMDobject& _object;
private:
	//シェーダに投げられるマテリアルデータ
	struct MaterialForHlsl {
		XMFLOAT3 diffuse;//ディフューズ色
		float alpha;	//ディフューズα
		XMFLOAT3 specular;//スペキュラ色
		float specularStrength;//スペキュラの強さ（乗算値）
		XMFLOAT3 ambient;//アンビエント色
	};

	//上記以外のマテリアルデータ
	struct AdditionalMaterial {
		string texpath;
		int toonIdx;
		bool edgeFlg;
	};

	//全体をまとめるデータ
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
	// デバイス
	static ID3D12Device* device;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeap;

	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;


	// 頂点バッファ,ビュー
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// インデックスバッファ,ビュー
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibView = {};

	//座標変換
	Transform transform;
	Transform* mappedTransform = nullptr;
	ComPtr<ID3D12Resource> transformBuff = nullptr;
	ComPtr<ID3D12Resource> transformMat = nullptr;
	ComPtr<ID3D12DescriptorHeap> transformHeap = nullptr;

	//マテリアル
	uint32_t materialNum;
	std::vector<Material>materials;
	ComPtr<ID3D12Resource> materialBuff;
	std::vector<ComPtr<ID3D12Resource>> textureResources;
	std::vector<ComPtr<ID3D12Resource>> sphResources;
	std::vector<ComPtr<ID3D12Resource>> spaResources;
	std::vector<ComPtr<ID3D12Resource>> toonResources;

	//ヒープ領域
	static ComPtr<ID3D12DescriptorHeap> materialDescHeap;

	std::vector<DirectX::XMMATRIX> boneMatrices;

private:

	ComPtr<ID3D12Resource> LoadTextureFromFile(string& texPath);

	ID3D12Resource* CreateWhiteTexture();
	ID3D12Resource* CreateBlackTexture();

	HRESULT CreateMaterial();

	//マテリアル及びテクスチャのビュー生成
	HRESULT CreateMaterialAndTextureView();

	HRESULT CreateTransform();

	HRESULT LoadPMDFile(const char* path);
	
public:
	PMDmodel(ID3D12Device* device, const char* filepath,PMDobject& object);
	~PMDmodel();

public:
	void Update();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	//アクセッサ
	ID3D12DescriptorHeap* MaterialDescHeap() { return materialDescHeap.Get(); }
	ID3D12DescriptorHeap* DescHeap() { return descHeap.Get(); }
	D3D12_VERTEX_BUFFER_VIEW VbView() { return vbView; }
	D3D12_INDEX_BUFFER_VIEW IbView() { return ibView; }
	std::vector<Material> Materials() { return materials; }

	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}

public://メンバ変数
	ComPtr<ID3D12Resource> PMDconstBuffB1; // 定数バッファ

	// 色
	XMFLOAT4 color = { 1,1,1,1 };
	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	// ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	// ローカルワールド変換行列
	XMMATRIX matWorld;

};