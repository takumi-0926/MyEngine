#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>

#include "Sprite/spriteCommon.h"
#include "Camera/Camera.h"
#include "pipelineSet.h"

class Wrapper;
class SceneEffect {
	static Wrapper* dx12;

private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
public: // サブクラス
	// 頂点データ構造体
	struct VertexPos {
		XMFLOAT3 pos;
		float scale;
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData {
		XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX matBillboard;
		XMFLOAT4 color;
	};

	//
	struct sceneEffect {
		XMFLOAT3 position = {};
		float scale = 1.0f;
		XMFLOAT4 color = { 0,0,0,1 };
	};

private: // 定数
	static const int division = 50;					// 分割数
	static const float radius;				// 底面の半径
	static const float prizmHeight;			// 柱の高さ
	static const int planeCount = division * 2 + division * 2;		// 面の数
	//static const int vertexCount = planeCount * 3;		// 頂点数

	//頂点数
	static const int vertexCount = 1024;
	static const int indexCount = 3 * 2;

public://静的メンバ関数
	static bool staticInitalize(ID3D12Device* _dev, SIZE ret);
	//パイプライン生成
	static bool Createpipeline(ID3D12Device* _dev, SIZE ret);
	//テクスチャ読み込み
	static bool loadTexture(UINT texNumber, const wchar_t* fileName);

	static void PreDraw(ID3D12GraphicsCommandList* _cmdList);

	static void PostDraw();

	/// <summary>
	/// ビュー行列を更新
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// カメラセット
	/// </summary>
	/// <param name="camera"></param>
	static inline void SetCamera(Camera* camera) { SceneEffect::camera = camera; }

	static SceneEffect* Create(
		UINT texNumber,
		XMFLOAT3 position,
		XMFLOAT4 color = { 1,1,1,1 },
		XMFLOAT2 anchorpoint = { 0.0f,0.0f },
		bool isFlipX = false, bool isFlipY = false);

	SceneEffect(
		UINT texNumber,
		XMFLOAT3 position,
		XMFLOAT2 size,
		XMFLOAT4 color,
		XMFLOAT2 anchorpoint,
		bool isFlipX, bool isFlipY);

private://メンバ関数

protected:
	//デバイス
	static ID3D12Device* device;
	//コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeap;

	static Camera* camera;
	static SpriteCommon spritecommon;
	static PipelineSet pipelineset;
	static XMMATRIX matBillboard;
	static XMMATRIX matBillboardY;

	// ビュー行列
	static XMMATRIX matView;
	// 射影行列
	static XMMATRIX matProjection;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// 頂点バッファ
	static ComPtr<ID3D12Resource> vertBuff;
	// テクスチャバッファ
	static ComPtr<ID3D12Resource> texbuff;
	// シェーダリソースビューのハンドル(CPU)
	static CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
	static CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

	std::forward_list<sceneEffect> effects;

protected:
	//頂点ばっふぁ、定数ばっふぁ
	ComPtr<ID3D12Resource> _vertbuff = nullptr;
	ComPtr<ID3D12Resource> constBuff = nullptr;
	//頂点ばっふぁびゅー
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	float rotation = 0.0f;	//回転角
	XMFLOAT3 position = { 0,0,0 };//座標
	XMMATRIX matWorld;	//ワールド行列
	XMFLOAT4 color = { 1,1,1,1 };	//色
	UINT texNumber = 0;	//テクスチャ番号
	XMFLOAT2 size = { 100.0f,100.0f };	//サイズ
	XMFLOAT2 anchorpoint = { 0.0f,0.5f };	//アンカーポイント
	bool isFlipX = false;//左右反転
	bool isFlipY = false;//上下反転
	XMFLOAT2 texLeftTop = { 0,0 };	//テクスチャ左上座標,始点
	XMFLOAT2 texSize = { 100.0f,100.0f };	//テクスチャ切り出しサイズ
	XMFLOAT2 texBase = { 0, 0 };	// テクスチャ始点
	bool isInvisible = false;	//非表示
	float alpha = 1.0f;

public:

	bool Initalize();
	virtual void Update();
	virtual void Draw();

	/// <summary>
	/// パーティクル生成
	/// </summary>
	/// <param name="emitter">生成場所</param>
	/// <param name="num">生成数</param>
	void CreateParticle(XMFLOAT3 emitter, XMFLOAT4 color);

public:
	void SetAlpha(float alpha) {
		this->alpha = alpha;
	}
	/// <summary>
	/// テクスチャ範囲設定
	/// </summary>
	/// <param name="texBase">テクスチャ左上座標</param>
	/// <param name="texSize">テクスチャサイズ</param>
	void SetTextureRect(XMFLOAT2 texBase, XMFLOAT2 texSize);

	XMFLOAT3 Pos() { return position; }
};
