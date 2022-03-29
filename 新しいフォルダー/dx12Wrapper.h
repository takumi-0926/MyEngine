#pragma once
#include "includes.h"
#include "application.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class Wrapper {
private:
	//ウィンドウズアプリケーション
	//Application* WinApp;
	
	//dx12基盤
	ComPtr<ID3D12Device>				_dev		 = nullptr;				//デバイス
	ComPtr<IDXGIFactory6>				_dxgifactory = nullptr;		//dxgiファクトリー
	//スワップチェーン
	ComPtr<IDXGISwapChain4>				_swapchain = nullptr;		//スワップチェーン
	//コマンド
	ComPtr<ID3D12CommandAllocator>		_cmdAllocator = nullptr;	// コマンドリストアケローター
	ComPtr<ID3D12GraphicsCommandList>	_cmdList	  = nullptr;			//コマンドリスト
	ComPtr<ID3D12CommandQueue>			_cmdQueue	  = nullptr;		//コマンドキュ
	//フェンス
	ComPtr<ID3D12Fence>					_fence	  = nullptr;			//フェンス
	UINT								_fenceval = 0;
	//表示関連
	ComPtr<ID3D12DescriptorHeap>		_rtvHeaps	   = nullptr;		//レンダヒープ
	ComPtr<ID3D12DescriptorHeap>		_basicDescHeap = nullptr;	//シェーダーヒープ
	vector<ComPtr<ID3D12Resource>>      _backBuffer;
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

	HRESULT InitializeDescHeap();
	
	HRESULT InitializeDepthBuff(SIZE ret);
	
	HRESULT InitializeFence();
	
	//ComPtr<ID3D12Device> Device();
	ID3D12Device* GetDevice() { return _dev.Get(); }
	ComPtr<IDXGISwapChain4> SwapChain();
	ComPtr<ID3D12GraphicsCommandList> CommandList();
};