#pragma once
#include "spriteCommon.h"
#include "pipelineSet.h"

class Sprite : LoadHlsls{
private://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//スプライト用頂点データ
	struct VertexPosUv {
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	/// <summary>
	/// 定数バッファ用データ構造体
	/// </summary>
	struct ConstBufferData
	{
		XMFLOAT4 color;	// 色 (RGBA)
		XMMATRIX mat;	// ３Ｄ変換行列
	};

public://静的メンバ関数
	static bool staticInitalize(ID3D12Device* _dev, SIZE ret);
	//パイプライン生成
	static bool Createpipeline(ID3D12Device* _dev, SIZE ret);
	//テクスチャ読み込み
	static bool loadTexture(UINT texNumber, const wchar_t* fileName);

	static void PreDraw(ID3D12GraphicsCommandList* _cmdList);

	static void PostDraw();

	static Sprite* Create(
		UINT texNumber,
		XMFLOAT3 position, 
		XMFLOAT4 color = { 1,1,1,1 }, 
		XMFLOAT2 anchorpoint = { 0.0f,0.0f }, 
		bool isFlipX = false, bool isFlipY = false);

	Sprite(
		UINT texNumber,
		XMFLOAT3 position,
		XMFLOAT2 size,
		XMFLOAT4 color,
		XMFLOAT2 anchorpoint,
		bool isFlipX, bool isFlipY);

private://メンバ関数
	/// <summary>
	/// 頂点データ転送
	/// </summary>
	void TransVertex();

private:
	//デバイス
	static ID3D12Device* device;
	//コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;

	static SpriteCommon spritecommon;

	static PipelineSet pipelineset;

private:
	//頂点ばっふぁ、定数ばっふぁ
	ComPtr<ID3D12Resource> _vertbuff = nullptr;
	ComPtr<ID3D12Resource> constBuff = nullptr;
	//頂点ばっふぁびゅー
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//回転角
	float rotation = 0.0f;
	//座標
	XMFLOAT3 position = { 0,0,0 };
	//ワールド行列
	XMMATRIX matWorld;

	//色
	XMFLOAT4 color = { 1,1,1,1 };

	//テクスチャ番号
	UINT texNumber = 0;

	//サイズ
	XMFLOAT2 size = { 100.0f,100.0f };

	//アンカーポイント
	XMFLOAT2 anchorpoint = { 0.0f,0.5f };

	bool isFlipX = false;//左右反転
	bool isFlipY = false;//上下反転

	//テクスチャ左上座標,始点
	XMFLOAT2 texLeftTop = { 0,0 };

	//テクスチャ切り出しサイズ
	XMFLOAT2 texSize = { 100.0f,100.0f };

	//非表示
	bool isInvisible = false;


public:

	bool Initalize();

	void Update();

	void Draw();
	
public:
	/// <summary>
	/// サイズ設定
	/// </summary>
	/// <param name="size">サイズ</param>
	void SetSize(XMFLOAT2 size) {
		this->size = size;
		TransVertex();
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(XMFLOAT3 pos) {
		this->position = pos;
		TransVertex();
	}

	void MovePos(XMFLOAT3 pos) {
		this->position.x += pos.x;
		this->position.y += pos.y;
		this->position.z += pos.z;
		TransVertex();
	}
	/// <summary>
	/// 
	/// </summary>
	void SetAnchorPoint(XMFLOAT2 anchorpoint) {
		this->anchorpoint = anchorpoint;
		TransVertex();
	}
};