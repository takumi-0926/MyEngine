#pragma once
#include <DirectXMath.h>
#include "includes.h"

#include "pipelineSet.h"
#include "PMDModel.h"

#include "baseObject.h"

class PMDModel;
class PMDobject : public BaseObject {
	friend PMDModel;
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	//std::省略
	using string = std::string;

private:
	//3Dオブジェクト用
	static ComPtr<ID3D12RootSignature>			_rootsignature;
	static ComPtr<ID3D12PipelineState>			_pipelinestate;

	HRESULT CreateGraphicsPipeline();

	HRESULT CreateRootSignature();

public:
	PMDobject();
	~PMDobject();

	// 静的初期化
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	/// <returns>成否</returns>
	static bool StaticInitialize(ID3D12Device* device, SIZE ret);

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static bool InitializeGraphicsPipeline();

	/// <summary>
	/// モデル生成
	/// </summary>
	/// <returns></returns>
	static PMDobject* Create();

	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}
public:
	//モデルセット
	void SetModel(PMDmodel* model);
	//初期化
	bool Initialize();
	//更新
	void Update();
	//描画
	void Draw();

private:
	ComPtr<ID3D12Resource> PMDconstBuffB0;
	ComPtr<ID3D12DescriptorHeap> _sceneDescHeap = nullptr;

public:
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

	PMDmodel* model = nullptr;
};
