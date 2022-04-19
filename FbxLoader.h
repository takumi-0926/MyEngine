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

	void LoadModelFromFile(const string& modelName);

	void ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュ読み込み
	/// </summary>
	/// <param name="model"></param>
	/// <param name="fbxNode"></param>
	void ParseMesh(FbxModel* model, FbxNode* fbxNode);

	void ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh);

	void ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh);

	void ParseMaterial(FbxModel* model, FbxNode* fbxNode);

	void LoadTexture(FbxModel* model, const std::string& fullPath);

	std::string ExtractFileName(const std::string& path);

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