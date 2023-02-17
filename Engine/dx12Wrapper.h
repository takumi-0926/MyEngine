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
	//ウィンドウズアプリケーション
	//Application* WinApp;

		// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0_1
	{
		//XMFLOAT4 color;	// 色 (RGBA)
		//XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX viewproj;
		XMMATRIX shadow;
		XMFLOAT3 cameraPos;
	};

	//dx12基盤
	ComPtr<ID3D12Device>				_dev = nullptr;				//デバイス
	ComPtr<IDXGIFactory6>				_dxgifactory = nullptr;		//dxgiファクトリー
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
	D3D12_CPU_DESCRIPTOR_HANDLE			heapHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			_heapHandle;
	//深度バッファ
	ComPtr<ID3D12DescriptorHeap>		_dsvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap>		_depthSRVHaep = nullptr;	//深度テクスチャ
	ComPtr<ID3D12Resource>				_depthBuffer = nullptr;
	//シーンを構成するバッファまわり
	ConstBufferDataB0_1*				_mappedSceneData;
	ComPtr<ID3D12Resource>				_sceneConstBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap>		_sceneDescHeap = nullptr;
	//imgui
	ComPtr<ID3D12DescriptorHeap>		_heapForImgui; //ヒープ保持用

	D3D12_DESCRIPTOR_HEAP_DESC          heapDesc = {};

	static Light* light;

	static Camera* camera;
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

public:
	~Wrapper();

	bool Init(HWND _hwnd, SIZE _ret);

	void PreRun();
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

	void SceneDraw() {
		//現在のシーン(ビュープロジェクション)をセット
		ID3D12DescriptorHeap* sceneheaps[] = { _descHeap.Get() };
		_cmdList->SetDescriptorHeaps(1, sceneheaps);
		_cmdList->SetGraphicsRootDescriptorTable(0, _descHeap->GetGPUDescriptorHandleForHeapStart());
	}
	void SceneUpdate() {
		HRESULT result;
		_mappedSceneData = nullptr;//マップ先を示すポインタ
		result = _sceneConstBuff->Map(0, nullptr, (void**)&_mappedSceneData);//マップ

		const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
		const XMFLOAT3& cameraPos = camera->GetEye();
		const XMFLOAT4 planeVec(0, 1, 0, 0);
		const XMFLOAT3 lightVec(1, -1, 1);

		_mappedSceneData->viewproj = matViewProjection;
		_mappedSceneData->shadow = XMMatrixShadow(
			XMLoadFloat4(&planeVec),
			-XMLoadFloat3(&lightVec));

		_mappedSceneData->cameraPos = cameraPos;

		_sceneConstBuff->Unmap(0, nullptr);
	}

	ID3D12Device* GetDevice() { return _dev.Get(); }
	ComPtr<IDXGISwapChain4> SwapChain();
	ComPtr<ID3D12GraphicsCommandList> CommandList();
	ID3D12DescriptorHeap* GetDescHeap() { return _descHeap.Get(); }
	ComPtr<ID3D12DescriptorHeap> GetHeapImgui() { return _heapForImgui.Get(); }
	Camera* Camera() { return camera; }

	static int lightNum;

};