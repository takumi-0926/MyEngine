#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include "includes.h"

class PMDmodel {
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

private:
	//ヘッダー
	struct PMDHeader {
		float vertion;
		char model_name[20];
		char comment[256];
	};

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

private:
	// デバイス
	static ID3D12Device* device;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeap;

	// 頂点バッファ,ビュー
	ComPtr<ID3D12Resource> vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファ,ビュー
	ComPtr<ID3D12Resource> indexBuff;
	D3D12_INDEX_BUFFER_VIEW ibView;

	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;

	ComPtr<ID3D12Resource> materialBuff;

	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(GPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;



	//ルートシグネチャ
	//static ID3D12RootSignature* rootsignature;

	//ヒープ領域
	static ComPtr<ID3D12DescriptorHeap> materialDescHeap;

	//マテリアル
	std::vector<Material>materials;
	//std::vector<PMDMaterial>pmdMaterials;
	uint32_t materialNum;

	std::vector<ComPtr<ID3D12Resource>> textureResources;
	std::vector<ComPtr<ID3D12Resource>> sphResources;
	std::vector<ComPtr<ID3D12Resource>> spaResources;

	// 頂点データ配列
	static std::vector<unsigned char> vertices;
	// 頂点インデックス配列	//static unsigned short indices[planeCount * 3];
	static std::vector<unsigned short> indices;

	static PMDHeader pmdheader;

	static TexMetadata metadata;
	static ScratchImage scratchImg;

private:
	//モデルのパスとテクスチャのパスから合成パスを得る
//@param modelpath アプリケーションから見たpmdモデルのパス
//@param texPath PMD モデルから見たテクスチャのパス
//@return アプリケーションから見たテクスチャのパス
	static string GetTexturePathFromModelAndTexPath(const string& modelPath, const char* texPath) {
		int pathIndex1 = modelPath.rfind('/');
		int pathIndex2 = modelPath.rfind('\\');

		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}

	//std::string{マルチバイト文字列}からstd::wstring{ワイド文字列}を得る
	//@param str マルチバイト文字列
	//@return 変換された文字列
	static std::wstring GetWideStringFromString(const string& str)
	{
		//呼び出し1回目(文字列数を得る)
		auto num1 = MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1, nullptr, 0);

		std::wstring wstr;//stringのwchar_t版
		wstr.resize(num1);//得られた文字列数でリサイズ

		//呼び出し2回目(確保済みのwstrに変換文字列をコピー)
		auto num2 = MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1, &wstr[0], num1);

		assert(num1 == num2);//チェック
		return  wstr;
	}

	ComPtr<ID3D12Resource> LoadTextureFromFile(string& texPath);

	ID3D12Resource* CreateWhiteTexture();

	ID3D12Resource* CreateBlackTexture();

	//ファイル名から拡張子を取得する
	//@param path 対象のパス文字列
	//@return 拡張子
	static string GetExtension(const string& path) {
		int idx = path.rfind('.');
		return path.substr(idx + 1, path.length() - idx - 1);
	}

	//テクスチャのパスをセパレーター文字で取得する
	//@param path 対象のパス文字列
	//@param splitter 区切り文字
	//@return 分離前後の文字列ペア
	static std::pair<string, string>SplitFileName(const string& path, const char splitter = '*') {
		int idx = path.find(splitter);
		std::pair<string, string>ret;
		ret.first = path.substr(0, idx);
		ret.second = path.substr((idx + 1), path.length() - idx - 1);

		return ret;
	}

public:
	PMDmodel();

	static PMDmodel* Create();

	//静的初期化
	static bool StaticInitialize(ID3D12Device* device);
	//ヒープ初期化
	static bool InitializeDescriptorHeap();

	//モデル読み込み
	void CreateModel(const std::string& strModelPath);

	//テクスチャ読み込み
	void Loadtexture();

	//マテリアル読み込み
	void LoadMaterial(const std::string& strModelPath);

public:

	bool Initialize();

	void Update();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	//アクセッサ
	std::vector<unsigned short> Indices() { return indices; }
	ID3D12DescriptorHeap* MaterialDescHeap() { return materialDescHeap.Get(); }
	ID3D12DescriptorHeap* DescHeap() { return descHeap.Get(); }
	D3D12_VERTEX_BUFFER_VIEW VbView() { return vbView; }
	D3D12_INDEX_BUFFER_VIEW IbView() { return ibView; }
	std::vector<Material> Materials() { return materials; }

public://メンバ変数
	ComPtr<ID3D12Resource> PMDconstBuffB1; // 定数バッファ
};