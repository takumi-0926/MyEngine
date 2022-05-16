#include <cassert>

#include "dx12Wrapper.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

//@brief コンソール画面にフォーマット付き文字列を表示
//@param formatフォーマット（%dとか%fとかの）
//@param 可変長引数
//@remarks この関数はデバック用です
void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf(format, valist);
	va_end(valist);
#endif
}

void EnableDebugLayer() {
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	debugLayer->EnableDebugLayer();
	debugLayer->Release();
}

Wrapper::~Wrapper()
{
	bool debug;
	debug = false;
}

bool Wrapper::Init(HWND _hwnd, SIZE _ret) {
#ifdef _DEBUG
	//エラーチェック//
	EnableDebugLayer();

	ComPtr<ID3D12DeviceRemovedExtendedDataSettings> dredSettings;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings)))); {
		dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	}

#endif

	//DirectX12関連初期化
	if (FAILED(InitializeDevice())) {
		assert(0);
		return false;
	}
	if (FAILED(InitializeCommand())) {
		assert(0);
		return false;
	}
	if (FAILED(InitializeSwapChain(_hwnd))) {
		assert(0);
		return false;
	}
	if (FAILED(InitializeDescHeap())) {
		assert(0);
		return false;
	}
	if (FAILED(InitializeDepthBuff(_ret))) {
		assert(0);
		return false;
	}
	if (FAILED(InitializeFence())) {
		assert(0);
		return false;
	}

	return true;
}

void Wrapper::PreRun()
{	//バックバッファのインデックスを取得
	auto bbIdx = SwapChain()->GetCurrentBackBufferIndex();

	//リソースバリア（Present⇔レンダターゲット）状態の変更
	_cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(_backBuffer[bbIdx].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

	//描画先を指定
	//レンダターゲットビュー用のディスクリプタヒープのハンドルを用意
	auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
	_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);

	//レンダターゲット(画面)のクリア
	float clearColor[] = { 0.1f,0.25f, 0.5f,0.0f };//青っぽい色
	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ビューポートの設定
	_cmdList->RSSetViewports(1, 
		&CD3DX12_VIEWPORT(0.0f, 0.0f, Application::window_width, Application::window_height));
	// シザリング矩形の設定
	_cmdList->RSSetScissorRects(1, 
		&CD3DX12_RECT(0, 0, Application::window_width, Application::window_height));
}

void Wrapper::PostRun() {
	//バックバッファのインデックスを取得
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();

	//リソースバリア（レンダターゲット⇔Present）状態の変更
	_cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(_backBuffer[bbIdx].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	//命令のクローズ
	_cmdList->Close();

	//コマンドリストの実行
	ID3D12CommandList* cmdlists[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(1, cmdlists);

	_swapchain->Present(1, 0);//Presentの実行

	//実行完了まで待機
	_cmdQueue->Signal(_fence.Get(), ++_fenceval);
	if (_fence->GetCompletedValue() != _fenceval) {
		auto event = CreateEvent(nullptr, false, false, nullptr);
		_fence->SetEventOnCompletion(_fenceval, event);
		//イベントが発生するまで待つ(INFINITE)
		WaitForSingleObject(event, INFINITE);
		//イベントハンドルを閉じる
		CloseHandle(event);
	}

	_cmdAllocator->Reset();//キューをクリア
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);//再びコマンドリストをためる準備
}

void Wrapper::Processing()
{
}

HRESULT Wrapper::result() {
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgifactory));
	return result;
}

HRESULT Wrapper::InitializeDevice() {
	auto _result = result();
	//DXGIファクトリーの生成
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	//アダプター列挙用
	vector<ComPtr<IDXGIAdapter>>adapters;
	//ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter> tmpAdapter = nullptr;
	//アダプター
	for (int i = 0; _dxgifactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);//アダプターの説明オブジェクト取得
		wstring strDesc = adesc.Description;
		//探したいアダプターの名前を確認
		if (strDesc.find(L"NVIDIA") != string::npos) {
			tmpAdapter = adpt;
			break;
		}
	}

	//Direct3Dデバイスの設定
	D3D_FEATURE_LEVEL featureLevel;
	//フィーチャーレベルの取得
	for (auto lv : levels) {
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK) {
			featureLevel = lv;
			_result = S_OK;
			break;
		}
	}

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(_dev->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	}

#endif // _DEBUG

	return _result;
}

HRESULT Wrapper::InitializeSwapChain(const HWND& _hwnd) {
	auto& app = Application::Instance();

	SIZE ret = app.GetWindowSize();

	//スワップチェーン生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.Width = ret.cx;
	swapchainDesc.Height = ret.cy;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;//バックバッファは伸び縮み可能
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//フリップ後は速やかに破棄
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;//特に指定なし
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//ウィンドウ⇔フルスクリーン切り替え可能

	auto result = _dxgifactory->CreateSwapChainForHwnd(
		_cmdQueue.Get(),
		_hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapchain.ReleaseAndGetAddressOf());

	return result;
}

HRESULT Wrapper::InitializeCommand() {
	//コマンドリスト、コマンドアロケーターの生成
	auto result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	////コマンドキューの生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;//タイムアウト無し
	cmdQueueDesc.NodeMask = 0;	//アダプターを一つしか使わないときは０でよい
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	//プライオリティーは特になし
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	//コマンドリストと合わせる

	//キューの作成
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));

	_cmdQueue->SetName(L"queue");

	return result;
}

HRESULT Wrapper::InitializeDescHeap() {
	//ディスクリプタヒープ生成(レンダ)
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//どんなビューを作るのか(今回はレンダターゲットなのでRTV)
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;//表裏の2つ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//特に指定なし

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	auto result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_rtvHeaps.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	result = _swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//先頭アドレスの設定
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	_backBuffer.resize(swcDesc.BufferCount);

	//SRGBレンダターゲットビュー設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < swcDesc.BufferCount; ++i) {
		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffer[i]));
		if (FAILED(result)) {
			assert(0);
			return result;
		}

		_backBuffer[i]->SetName(L"buffer");

		//rtvDesc.Format = _backBuffer[i]->GetDesc().Format;
		//レンダターゲットビューの生成
		_dev->CreateRenderTargetView(_backBuffer[i].Get(), &rtvDesc, handle);
		//ポインターをずらす
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	return result;
}

HRESULT Wrapper::InitializeDepthBuff(SIZE ret) {
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元のテクスチャデータ
	depthResDesc.Width = ret.cx;//幅と高さはレンダターゲットと同じ
	depthResDesc.Height = ret.cy;//幅と高さはレンダターゲットと同じ
	depthResDesc.DepthOrArraySize = 1;//テクスチャ配列でも3Dテクスチャでもない
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値書き込み用フォーマット
	depthResDesc.SampleDesc.Count = 1;//サンプルは1ピクセル当たりの一つ
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//デプスステンシルとして使用

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;//デフォルトなので以下UNKNOWN
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	//クリアバリュー（とても重要）
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f;//深さ1.0fでクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//32ビットfloat値としてクリア

	auto result = _dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度書き込み用
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//深度のためのディスクリプタヒープ
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//深度ビュー生成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	_dev->CreateDepthStencilView(
		_depthBuffer.Get(),
		&dsvDesc,
		_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

HRESULT Wrapper::InitializeFence() {
	//フェンス生成
	auto result = _dev->CreateFence(_fenceval, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf()));

	return result;
}

//ComPtr<ID3D12Device> Wrapper::Device() {
//	return _dev;
//}

ComPtr<IDXGISwapChain4> Wrapper::SwapChain() {
	return _swapchain;
}

ComPtr<ID3D12GraphicsCommandList> Wrapper::CommandList() {
	return _cmdList;
}
