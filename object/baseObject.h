#pragma once
#include <DirectXMath.h>
#include "..\includes.h"

#include "..\pipelineSet.h"
#include "..\Camera\Camera.h"

//シェーダに渡すための行列データ
struct constBufferData {
	XMFLOAT4 color;//色(RGBA)
	XMMATRIX mat;
};

//バッファデータ数
const int constantBufferNum = 128;

class BaseObject{
protected:
	//パイプライン / ルートシグネチャ
	//static ComPtr<ID3D12PipelineState>			_pipelinestate;
	//static ComPtr<ID3D12RootSignature>			_rootsignature;

	//シェーダ
	ID3DBlob* _vsBlob = nullptr; //頂点シェーダ用
	ID3DBlob* _psBlob = nullptr; //ピクセルシェーダ用
	ID3DBlob* _gsBlob = nullptr; //ジオメトリシェーダ用

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

	// ビュー行列
	static XMMATRIX matView;
	// 射影行列
	static XMMATRIX matProjection;
	// 視点座標
	static XMFLOAT3 eye;
	// 注視点座標
	static XMFLOAT3 target;
	// 上方向ベクトル
	static XMFLOAT3 up;

	ComPtr<ID3D12DescriptorHeap>		_DescHeap = nullptr;	//汎用ヒープ

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		//XMFLOAT4 color;	// 色 (RGBA)
		//XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
		XMMATRIX pmdWorld;
	};
	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0_1
	{
		//XMFLOAT4 color;	// 色 (RGBA)
		//XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX viewproj;
		XMFLOAT3 cameraPos;
	};


protected: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	//パイプライン
	static PipelineSet pipelineSet;

	//static Camera* camera;

public:
	// 描画前処理
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList) {
		// PreDrawとPostDrawがペアで呼ばれていなければエラー
		assert(BaseObject::cmdList == nullptr);

		// コマンドリストをセット
		BaseObject::cmdList = cmdList;

		// プリミティブ形状を設定
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	// 描画後処理
	static void PostDraw() {
		// コマンドリストを解除
		BaseObject::cmdList = nullptr;
	}

	////カメラ初期化
	//static void InitalizeCamera(int window_width, int window_height) {
	//	// ビュー行列の生成
	//	matView = XMMatrixLookAtLH(
	//		XMLoadFloat3(&eye),
	//		XMLoadFloat3(&target),
	//		XMLoadFloat3(&up));

	//	// 平行投影による射影行列の生成
	//	//constMap->mat = XMMatrixOrthographicOffCenterLH(
	//	//	0, window_width,
	//	//	window_height, 0,
	//	//	0, 1);
	//	// 透視投影による射影行列の生成
	//	matProjection = XMMatrixPerspectiveFovLH(
	//		XMConvertToRadians(60.0f),
	//		(float)window_width / window_height,
	//		0.1f, 1000.0f
	//	);
	//}

public:
	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	//static void SetCamera(Camera* camera) {
	//	BaseObject::camera = camera;
	//}
};