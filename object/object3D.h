#pragma once
#include <DirectXMath.h>

#include "baseObject.h"
#include "Model.h"
#include "..\includes.h"
#include "..\pipelineSet.h"
#include "..\PMD\PMDmodel.h"
#include "..\Collision\CollisionInfo.h"

//3Dオブジェクト用頂点データ
struct  Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

class Wrapper;
class BaseCollider;
class Object3Ds : public BaseObject
{
	static Wrapper* dx12;
protected: // メンバ変数

	const char* name = nullptr;

	BaseCollider* collider = nullptr;
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Object3Ds() = default;
	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~Object3Ds();
	// 静的初期化
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	/// <returns>成否</returns>
	static bool StaticInitialize(ID3D12Device* _device);

	// 3Dオブジェクト生成
	static Object3Ds* Create();

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static bool InitializeGraphicsPipeline();

	//モデルセット
	void SetModel(Model* _model);
public:
	// 毎フレーム処理
	virtual bool Initialize();
	// 毎フレーム処理
	virtual void Update();
	// 描画
	virtual void Draw();

	/// <summary>
	/// ワールド行列取得
	/// </summary>
	/// <returns></returns>
	const XMMATRIX GetMatWorld() { return matWorld; }

	void SetCollider(BaseCollider* collider);

	virtual void OnCollision(const CollisionInfo& info){}
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
	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}
};