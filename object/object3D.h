#pragma once
#include <DirectXMath.h>

#include "baseObject.h"
#include "Model.h"
#include "..\includes.h"
#include "..\pipelineSet.h"
#include "..\PMD\PMDmodel.h"

//3Dオブジェクト用頂点データ
struct  Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

class Wrapper;
class Object3Ds : public BaseObject
{
	static Wrapper* dx12;
private: // 静的メンバ変数
	//3Dオブジェクト用
	//static ComPtr<ID3D12RootSignature>			_rootsignature;
	//static ComPtr<ID3D12PipelineState>			_pipelinestate;

public:
	// 静的初期化
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	/// <returns>成否</returns>
	static bool StaticInitialize(ID3D12Device* _device, SIZE _ret);

	// 3Dオブジェクト生成
	static Object3Ds* Create();

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static bool InitializeGraphicsPipeline();

public:
	//モデルセット
	void SetModel(Model* model);
	// 毎フレーム処理
	virtual bool Initialize();
	// 毎フレーム処理
	virtual void Update();
	// 描画
	virtual void Draw();

public:
	ComPtr<ID3D12Resource> constBuffB0; // 定数バッファ(OBJ)
	ComPtr<ID3D12Resource> constBuffB1; // 定数バッファ(OBJ)

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

	Model* model = nullptr;

public:
	static void MoveEyeTarget(XMFLOAT3 s) {
		eye.x += s.x;
		eye.y += s.y;
		eye.z += s.z;
	}
	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}
};