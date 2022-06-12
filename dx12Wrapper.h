#pragma once
#include "includes.h"
#include "application.h"

class Wrapper {
public:
	//namespace省略
	//エイリアス
	template <class T> using Comptr = Microsoft::WRL::ComPtr<T>;

private:
	//ウィンドウズアプリケーション
	//Application* WinApp;
	
	//dx12基盤
	ComPtr<ID3D12Device>				_dev		 = nullptr;		//デバイス
	ComPtr<IDXGIFactory6>				_dxgifactory = nullptr;		//dxgiファクトリー
	//スワップチェーン
	ComPtr<IDXGISwapChain4>				_swapchain = nullptr;		//スワップチェーン
	//コマンド
	ComPtr<ID3D12CommandAllocator>		_cmdAllocator = nullptr;	// コマンドリストアケローター
	ComPtr<ID3D12GraphicsCommandList>	_cmdList	  = nullptr;	//コマンドリスト
	ComPtr<ID3D12CommandQueue>			_cmdQueue	  = nullptr;	//コマンドキュ
	//フェンス
	ComPtr<ID3D12Fence>					_fence	  = nullptr;		//フェンス
	UINT								_fenceval = 0;
	//表示関連
	vector<ComPtr<ID3D12Resource>>      _backBuffer;
	ComPtr<ID3D12DescriptorHeap>		_rtvHeaps	   = nullptr;	//レンダヒープ
	ComPtr<ID3D12DescriptorHeap>		_basicDescHeap = nullptr;	//汎用ヒープ
	D3D12_CPU_DESCRIPTOR_HANDLE			heapHandle;
	//深度バッファ
	ComPtr<ID3D12DescriptorHeap>		_dsvHeap	 = nullptr;
	ComPtr<ID3D12Resource>				_depthBuffer = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC          heapDesc = {};
public:
	~Wrapper();

	bool Init(HWND _hwnd,SIZE _ret);

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
	
	HRESULT InitializeDepthBuff(SIZE ret);
	
	HRESULT InitializeFence();
	
	void ClearDepthBuffer() {
		// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		// 深度バッファのクリア
		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	}

	//ComPtr<ID3D12Device> Device();
	ID3D12Device* GetDevice() { return _dev.Get(); }
	ComPtr<IDXGISwapChain4> SwapChain();
	ComPtr<ID3D12GraphicsCommandList> CommandList();
	ComPtr<ID3D12DescriptorHeap> GetDescHeap() { return _basicDescHeap.Get(); }
};