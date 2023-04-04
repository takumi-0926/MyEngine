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

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		//XMFLOAT4 color;	// 色 (RGBA)
		//XMMATRIX mat;	// ３Ｄ変換行列
		//XMMATRIX view;
		//XMMATRIX proj;
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
		XMMATRIX lightCamera;
	};
	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0_1
	{
		//XMFLOAT4 color;	// 色 (RGBA)
		//XMMATRIX mat;	// ３Ｄ変換行列
		//XMMATRIX view;
		//XMMATRIX proj;
		XMMATRIX viewproj;
		XMFLOAT3 cameraPos;
	};

protected: // 静的メンバ変数
	// デバイス
	static ComPtr<ID3D12Device> device;
	// コマンドリスト
	static ComPtr<ID3D12GraphicsCommandList> cmdList;

	// ビュー行列
	static XMMATRIX matView;
	// 射影行列
	static XMMATRIX matProjection;

public:
	// 描画前処理
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	// 描画後処理
	static void PostDraw();
};