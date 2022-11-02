#pragma once

#include "fbxsdk.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <string>

#include "FbxModel.h"

class FbxLoader
{
private:
	using string = std::string;

public:
	static const string baseDirectory;

	static const string defaultTextureFileName;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	//初期化
	void Initialize(ID3D12Device* device);

	//後始末
	void Finalize();

	FbxModel* LoadModelFromFile(const string& modelName);

	void ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュ読み込み
	/// </summary>
	/// <param name="model"></param>
	/// <param name="fbxNode"></param>
	void ParseMesh(FbxModel* model, FbxNode* fbxNode);

	//面を構成するデータの読み取り
	void ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh);
	//頂点座標の読み取り
	void ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh);
	//マテリアルの読み取り
	void ParseMaterial(FbxModel* model, FbxNode* fbxNode);
	//テクスチャの読み取り
	void LoadTexture(FbxModel* model, const std::string& fullPath);
	//スキニング情報の読み取り
	void ParseSkin(FbxModel* model, FbxMesh* fbxMesh);

	std::string ExtractFileName(const std::string& path);

	/// <summary>
	/// FBX行列をXMMatrixに変換
	/// </summary>
	/// <param name="dst">書き込み先</param>
	/// <param name="src">元となるFBX行列</param>
	static void ConvertMatrixFormFbx(DirectX::XMMATRIX* dst, const FbxMatrix& src);

private:
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

	ID3D12Device* device = nullptr;

	FbxManager* fbxManager = nullptr;

	FbxImporter* fbxImporter = nullptr;
};