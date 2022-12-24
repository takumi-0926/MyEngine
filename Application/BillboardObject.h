#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

#include <dx12Wrapper.h>
#include <forward_list>

//ビルボードオブジェクト
class BillboardObject
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//3Dオブジェクト用頂点データ
	struct  BillboardVertex {
		DirectX::XMFLOAT3 pos;
		float scale;
	};

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		DirectX::XMMATRIX viewproj;
		DirectX::XMMATRIX matBillboard;
	};
	//object１つ
	struct Object {
		DirectX::XMFLOAT3 position = {};
		float scale = 10.0f;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BillboardObject(UINT texNumber, XMFLOAT2 size);
	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~BillboardObject();
	/// 静的初期化
	/// <param name="device">デバイス</param>
	/// <returns>成否</returns>
	static bool StaticInitialize(ID3D12Device * _device);

	static bool InitializeGraphicsPipeline();

	// 3Dオブジェクト生成
	static BillboardObject* Create(UINT _texNumber);

	static bool LoadTexture(UINT texNumber,const wchar_t* FlieName);

	// 毎フレーム処理
	virtual bool Initialize();
	// 毎フレーム処理
	virtual void Update();
	// 描画
	virtual void Draw(ID3D12GraphicsCommandList* cmdList);

	//行列の更新
	void UpdateWorldMatrix();

	/// <summary>
	/// 頂点バッファの生成
	/// </summary>
	void CreateVertexBuffers();
	/// <summary>
	/// インデックスバッファの生成
	/// </summary>
	void CreateIndexBuffers();

private:
	static Wrapper* dx12;

	static const int vertexCount = 1;
	static const int indexCount = 3 * 2;
	// デバイス
	static Microsoft::WRL::ComPtr<ID3D12Device> device;
	// コマンドリスト
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;

	//テクスチャ最大枚数
	static const int objectSRVCount = 512;
	//テクスチャ用デスクリプタヒープの生成
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;
	//テクスチャリソースの配列
	static Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[objectSRVCount];
	// シェーダリソースビューのハンドル(CPU)
	static CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
	static CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;

	//頂点バッファ
	static ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	static ComPtr<ID3D12Resource> indexBuff;

	// 頂点バッファビュー
	static D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファビュー
	static D3D12_INDEX_BUFFER_VIEW ibView;
	// 頂点データ配列
	static BillboardVertex vertices[vertexCount];
	// 頂点インデックス配列
	static unsigned short indices[indexCount];

	// ローカルワールド変換行列
	XMMATRIX matScale = {};
	XMMATRIX matRot = {};
	XMMATRIX matTrans = {};
	XMMATRIX matWorld = {};

	// 視点座標
	static XMFLOAT3 eye;
	// 注視点座標
	static XMFLOAT3 target;
	// 上方向ベクトル
	static XMFLOAT3 up;

	// ビュー行列
	static DirectX::XMMATRIX matView;
	// 射影行列
	static DirectX::XMMATRIX matProjection;

	static XMMATRIX matBillboard;

	static XMMATRIX matBillboardY;

	std::forward_list<Object> objects;

public:
	ComPtr<ID3D12Resource> constBuffB0; // 定数バッファ(OBJ)

	// ローカル座標
	DirectX::XMFLOAT3 position = { 0,0,0 };
	// 色
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	// ローカルスケール
	float scale = 10.0f;
	// X,Y,Z軸回りのローカル回転角
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	//テクスチャ番号
	UINT texNumber = 0;
	//サイズ
	DirectX::XMFLOAT2 size = { 100.0f,100.0f };
	//アンカーポイント
	DirectX::XMFLOAT2 anchorpoint = { 0.0f,0.5f };
	//非表示
	bool isInvisible = false;

public:
	void SetEye(XMFLOAT3 eye);
	void SetTarget(XMFLOAT3 target);
};