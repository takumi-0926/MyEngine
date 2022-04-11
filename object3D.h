#pragma once
#include <DirectXMath.h>
#include "includes.h"

#include "pipelineSet.h"
#include "Model.h"
#include "PMDmodel.h"
#include "Camera.h"

#include "baseObject.h"

//3Dオブジェクト用頂点データ
struct  Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

//3Dオブジェクト構造体
//struct Object3D {
//	ComPtr<ID3D12Resource> constBuff = nullptr;
//	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
//	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
//
//	//アフィン変換行列
//	XMFLOAT3 scale = { 1,1,1 };//スケーリング倍率
//	XMFLOAT3 rotation = { 0,0,0 };//回転角
//	XMFLOAT3 position = { 0,0,0 };//座標
//
//	XMMATRIX worldMat;
//
//	Object3D* parent = nullptr;
//
//	bool Flag;
//};

////シェーダに渡すための行列データ
//struct constBufferData {
//	XMFLOAT4 color;//色(RGBA)
//	XMMATRIX mat;
//};
//
//const int constantBufferNum = 128;

class Object3Ds : public BaseObject
{
private: // 静的メンバ変数
	//3Dオブジェクト用
	static ComPtr<ID3D12RootSignature>			_rootsignature;
	//static ComPtr<ID3DBlob>						_rootSigBlob;
	static ComPtr<ID3D12PipelineState>			_pipelinestate;

public:
	// 静的初期化
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	/// <returns>成否</returns>
	static bool StaticInitialize(ID3D12Device* device, SIZE ret);

	// 3Dオブジェクト生成
	static Object3Ds* Create();

	//static void InitalizeCamera(int window_width, int window_height);

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static bool InitializeGraphicsPipeline();

	///// <summary>
	///// カメラのセット
	///// </summary>
	///// <param name="camera">カメラ</param>
	//static void SetCamera(Camera* camera) {
	//	Object3Ds::camera = camera;
	//}

	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}
public:
	////パイプライン生成
	//PipelineSet CreatePipeline();

	//3Dオブジェクト生成
	//void CreateObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap);

	//モデルセット
	void SetModel(Model* model);

	//void SetPmdModel(PMDmodel* pmdModel);

	// 毎フレーム処理
	bool Initialize();
	// 毎フレーム処理
	void Update();
	// 描画
	void Draw();

public:
	ComPtr<ID3D12Resource> constBuffB0; // 定数バッファ(OBJ)
	ComPtr<ID3D12Resource> constBuffB1; // 定数バッファ(OBJ)

	//ComPtr<ID3D12Resource> PMDconstBuffB0; // 定数バッファ(PMD)

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

	//PMDmodel* pmdModel = nullptr;
public:
	static void MoveEyeTarget(XMFLOAT3 s) {
		eye.x += s.x;
		eye.y += s.y;
		eye.z += s.z;
	}
};