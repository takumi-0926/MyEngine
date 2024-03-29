#pragma once
#include "application.h"
#include "includes.h"
#include "Camera/Camera.h"
#include "light\Light.h"

#include "imgui\imgui.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_impl_dx12.h"

class Wrapper {
public:
	//namespace省略
	//エイリアス
	template <class T> using Comptr = Microsoft::WRL::ComPtr<T>;
	
private:

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0_1
	{
		//XMFLOAT4 color;	// 色 (RGBA)
		//XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX viewproj;
		XMMATRIX lightCamera;
		XMMATRIX shadow;
		XMFLOAT3 cameraPos;
	};

	//dx12基盤
	//ComPtr<ID3D12Device>				_dev = nullptr;				//デバイス
	//ComPtr<IDXGIFactory6>				_dxgifactory = nullptr;		//dxgiファクトリー
	//スワップチェーン
	ComPtr<IDXGISwapChain4>				_swapchain = nullptr;		//スワップチェーン
	//コマンド
	ComPtr<ID3D12CommandAllocator>		_cmdAllocator = nullptr;	// コマンドリストアケローター
	ComPtr<ID3D12GraphicsCommandList>	_cmdList = nullptr;			//コマンドリスト
	ComPtr<ID3D12CommandQueue>			_cmdQueue = nullptr;		//コマンドキュ
	//フェンス
	ComPtr<ID3D12Fence>					_fence = nullptr;			//フェンス
	UINT								_fenceval = 0;
	//表示関連
	vector<ComPtr<ID3D12Resource>>      _backBuffer;
	ComPtr<ID3D12DescriptorHeap>		_rtvHeaps = nullptr;		//レンダヒープ
	ComPtr<ID3D12DescriptorHeap>		_descHeap = nullptr;		//汎用ヒープ
	/// <summary>
	/// 0 : シーン
	/// 1 : 通常深度
	/// 2 : ライト深度
	/// 3 : imGui
	/// 4 : ポストエフェクト(SRV)
	/// 5 : ポストエフェクト(深度SRV)
	/// 6 : 
	/// </summary>

	D3D12_CPU_DESCRIPTOR_HANDLE			heapHandle_CPU;
	D3D12_GPU_DESCRIPTOR_HANDLE			heapHandle_GPU;
	//深度バッファ
	ComPtr<ID3D12DescriptorHeap>		_dsvHeap = nullptr;
	ComPtr<ID3D12Resource>				_depthBuffer = nullptr;
	ComPtr<ID3D12Resource>				_lightDepthBuffer = nullptr;
	//シーンを構成するバッファまわり
	ConstBufferDataB0_1*				_mappedSceneData;
	ComPtr<ID3D12Resource>				_sceneConstBuff = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC          heapDesc = {};

	static Light* light;

	static Camera* camera;
	static Camera* depthCamera;
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
	//カメラ初期化
	static void InitalizeCamera(int window_width, int window_height) {
		// ビュー行列の生成
		matView = XMMatrixLookAtLH(
			XMLoadFloat3(&eye),
			XMLoadFloat3(&target),
			XMLoadFloat3(&up));

		// 平行投影による射影行列の生成
		//constMap->mat = XMMatrixOrthographicOffCenterLH(
		//	0, window_width,
		//	window_height, 0,
		//	0, 1);
		// 透視投影による射影行列の生成
		matProjection = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(60.0f),
			(float)window_width / window_height,
			0.1f, 1000.0f
		);
	}
	static void InitalizeLightCamera(int window_width, int window_height) {
		// ビュー行列の生成
		matView = XMMatrixLookAtLH(
			XMLoadFloat3(&eye),
			XMLoadFloat3(&target),
			XMLoadFloat3(&up));

		// 平行投影による射影行列の生成
		matProjection = XMMatrixOrthographicOffCenterLH(
			0, float(window_width),
			float(window_height), 0,
			0, 1);
		// 透視投影による射影行列の生成
		//matProjection = XMMatrixPerspectiveFovLH(
		//	XMConvertToRadians(60.0f),
		//	(float)window_width / window_height,
		//	0.1f, 1000.0f
		//);
	}

public:
	~Wrapper();
	static Wrapper* GetInstance();
	bool Init(HWND _hwnd, SIZE _ret);
	void Finalize();

	void PreRun();
	void PreRunShadow();
	void PostRun();

	//後処理
	void Processing();

	HRESULT result();

	HRESULT InitializeDevice();

	HRESULT InitializeSwapChain(const HWND& _hwnd);

	HRESULT InitializeCommand();

	HRESULT InitializeRenderHeap();

	HRESULT InitializeDescHeap();

	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeapForImgui();

	HRESULT InitializeDepthBuff(SIZE ret);

	HRESULT InitializeFence();

	HRESULT CreateSceneView();

	void ClearDepthBuffer() {
		// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		// 深度バッファのクリア
		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}

	void ClearDepthShadow();

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	static void SetCamera(Camera* camera) {
		Wrapper::camera = camera;
	}

	static void SetLight(Light* light) {
		Wrapper::light = light;
	}

	void DrawLight(ID3D12GraphicsCommandList* cmdlist);
	void DrawDepth();
	void SceneDraw();
	void SceneUpdate();

	//ID3D12Device* GetDevice() { return _dev.Get(); }
	ComPtr<IDXGISwapChain4> SwapChain();
	ComPtr<ID3D12GraphicsCommandList> CommandList();
	ComPtr<ID3D12DescriptorHeap> GetDescHeap() { return _descHeap.Get(); }
	static Camera* GetCamera() { return camera; }
	static Light* GetLight() { return light; }
	static int lightNum;

};