#include <cassert>

#include "dx12Wrapper.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

//@brief �R���\�[����ʂɃt�H�[�}�b�g�t���������\��
//@param format�t�H�[�}�b�g�i%d�Ƃ�%f�Ƃ��́j
//@param �ϒ�����
//@remarks ���̊֐��̓f�o�b�N�p�ł�
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
	//�G���[�`�F�b�N//
	EnableDebugLayer();

	ComPtr<ID3D12DeviceRemovedExtendedDataSettings> dredSettings;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings)))); {
		dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	}

#endif

	//DirectX12�֘A������
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
{	//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
	auto bbIdx = SwapChain()->GetCurrentBackBufferIndex();

	//���\�[�X�o���A�iPresent�̃����_�^�[�Q�b�g�j��Ԃ̕ύX
	_cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(_backBuffer[bbIdx].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

	//�`�����w��
	//�����_�^�[�Q�b�g�r���[�p�̃f�B�X�N���v�^�q�[�v�̃n���h����p��
	auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
	_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);

	//�����_�^�[�Q�b�g(���)�̃N���A
	float clearColor[] = { 0.1f,0.25f, 0.5f,0.0f };//���ۂ��F
	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// �r���[�|�[�g�̐ݒ�
	_cmdList->RSSetViewports(1, 
		&CD3DX12_VIEWPORT(0.0f, 0.0f, Application::window_width, Application::window_height));
	// �V�U�����O��`�̐ݒ�
	_cmdList->RSSetScissorRects(1, 
		&CD3DX12_RECT(0, 0, Application::window_width, Application::window_height));
}

void Wrapper::PostRun() {
	//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();

	//���\�[�X�o���A�i�����_�^�[�Q�b�g��Present�j��Ԃ̕ύX
	_cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(_backBuffer[bbIdx].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	//���߂̃N���[�Y
	_cmdList->Close();

	//�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdlists[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(1, cmdlists);

	_swapchain->Present(1, 0);//Present�̎��s

	//���s�����܂őҋ@
	_cmdQueue->Signal(_fence.Get(), ++_fenceval);
	if (_fence->GetCompletedValue() != _fenceval) {
		auto event = CreateEvent(nullptr, false, false, nullptr);
		_fence->SetEventOnCompletion(_fenceval, event);
		//�C�x���g����������܂ő҂�(INFINITE)
		WaitForSingleObject(event, INFINITE);
		//�C�x���g�n���h�������
		CloseHandle(event);
	}

	_cmdAllocator->Reset();//�L���[���N���A
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);//�ĂуR�}���h���X�g�����߂鏀��
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
	//DXGI�t�@�N�g���[�̐���
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	//�A�_�v�^�[�񋓗p
	vector<ComPtr<IDXGIAdapter>>adapters;
	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr<IDXGIAdapter> tmpAdapter = nullptr;
	//�A�_�v�^�[
	for (int i = 0; _dxgifactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);//�A�_�v�^�[�̐����I�u�W�F�N�g�擾
		wstring strDesc = adesc.Description;
		//�T�������A�_�v�^�[�̖��O���m�F
		if (strDesc.find(L"NVIDIA") != string::npos) {
			tmpAdapter = adpt;
			break;
		}
	}

	//Direct3D�f�o�C�X�̐ݒ�
	D3D_FEATURE_LEVEL featureLevel;
	//�t�B�[�`���[���x���̎擾
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

	//�X���b�v�`�F�[������
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.Width = ret.cx;
	swapchainDesc.Height = ret.cy;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;//�o�b�N�o�b�t�@�͐L�яk�݉\
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//�t���b�v��͑��₩�ɔj��
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;//���Ɏw��Ȃ�
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//�E�B���h�E�̃t���X�N���[���؂�ւ��\

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
	//�R�}���h���X�g�A�R�}���h�A���P�[�^�[�̐���
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

	////�R�}���h�L���[�̐���
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;//�^�C���A�E�g����
	cmdQueueDesc.NodeMask = 0;	//�A�_�v�^�[��������g��Ȃ��Ƃ��͂O�ł悢
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	//�v���C�I���e�B�[�͓��ɂȂ�
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	//�R�}���h���X�g�ƍ��킹��

	//�L���[�̍쐬
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));

	_cmdQueue->SetName(L"queue");

	return result;
}

HRESULT Wrapper::InitializeDescHeap() {
	//�f�B�X�N���v�^�q�[�v����(�����_)
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//�ǂ�ȃr���[�����̂�(����̓����_�^�[�Q�b�g�Ȃ̂�RTV)
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;//�\����2��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//���Ɏw��Ȃ�

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

	//�擪�A�h���X�̐ݒ�
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	_backBuffer.resize(swcDesc.BufferCount);

	//SRGB�����_�^�[�Q�b�g�r���[�ݒ�
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
		//�����_�^�[�Q�b�g�r���[�̐���
		_dev->CreateRenderTargetView(_backBuffer[i].Get(), &rtvDesc, handle);
		//�|�C���^�[�����炷
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	return result;
}

HRESULT Wrapper::InitializeDepthBuff(SIZE ret) {
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2�����̃e�N�X�`���f�[�^
	depthResDesc.Width = ret.cx;//���ƍ����̓����_�^�[�Q�b�g�Ɠ���
	depthResDesc.Height = ret.cy;//���ƍ����̓����_�^�[�Q�b�g�Ɠ���
	depthResDesc.DepthOrArraySize = 1;//�e�N�X�`���z��ł�3D�e�N�X�`���ł��Ȃ�
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�l�������ݗp�t�H�[�}�b�g
	depthResDesc.SampleDesc.Count = 1;//�T���v����1�s�N�Z��������̈��
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//�f�v�X�X�e���V���Ƃ��Ďg�p

	//�[�x�l�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;//�f�t�H���g�Ȃ̂ňȉ�UNKNOWN
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	//�N���A�o�����[�i�ƂĂ��d�v�j
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f;//�[��1.0f�ŃN���A
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//32�r�b�gfloat�l�Ƃ��ăN���A

	auto result = _dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//�[�x�������ݗp
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�[�x�̂��߂̃f�B�X�N���v�^�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�[�x�r���[����
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
	//�t�F���X����
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
