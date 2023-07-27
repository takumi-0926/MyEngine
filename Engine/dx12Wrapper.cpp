#include <cassert>

#include "dx12Wrapper.h"

#include "Singleton_Heap.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

Camera* Wrapper::camera;
Camera* Wrapper::depthCamera;
// �r���[�s��
XMMATRIX Wrapper::matView{};
// �ˉe�s��
XMMATRIX Wrapper::matProjection{};
// ���_���W
XMFLOAT3 Wrapper::eye = { -10.0f, 0, -10.0f };
// �����_���W
XMFLOAT3 Wrapper::target = { 0,0,0 };
// ������x�N�g��
XMFLOAT3 Wrapper::up = { 0,1,0 };

Light* Wrapper::light = { nullptr };
int Wrapper::lightNum = 0;

//���C�g�f�v�X�l
constexpr uint32_t shadow_difinition = 1024;

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

Wrapper* Wrapper::GetInstance()
{
	static Wrapper* instance = new Wrapper();
	return instance;
}

bool Wrapper::Init(HWND _hwnd, SIZE _ret) {
#ifdef _DEBUG
	//�G���[�`�F�b�N//
	EnableDebugLayer();

	ComPtr<ID3D12DeviceRemovedExtendedDataSettings> dredSettings;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings)))) {
		dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	}

#endif
	InitalizeCamera(_ret.cx, _ret.cy);
	//�J����������
	Camera* cam = new Camera(0, 0);
	SetCamera(cam);

	//�V���O���g���ȃf�o�C�X�A�f�B�X�N�q�[�v�̐���
	Singleton_Heap::GetInstance()->CreateDevice();
	Singleton_Heap::GetInstance()->CreateDescHeap();

	//DirectX12�֘A������
	//if (FAILED(InitializeDevice())) {
	//	assert(0);
	//	return false;
	//}
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
	if (FAILED(CreateSceneView())) {
		assert(0);
		return false;
	}
	if (FAILED(InitializeRenderHeap())) {
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
	//_heapForImgui = CreateDescriptorHeapForImgui();
	//if (_heapForImgui == nullptr) {
	//	assert(0);
	//	return false;
	//}

	return true;
}

void Wrapper::PreRun()
{	//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
	auto bbIdx = SwapChain()->GetCurrentBackBufferIndex();

	//���\�[�X�o���A�iPresent�̃����_�^�[�Q�b�g�j��Ԃ̕ύX
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffer[bbIdx].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	_cmdList->ResourceBarrier(1, &barrier);

	//�`�����w��
	//�����_�^�[�Q�b�g�r���[�p�̃f�B�X�N���v�^�q�[�v�̃n���h����p��
	auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * Singleton_Heap::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(heapDesc.Type);
	auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
	_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);

	//�����_�^�[�Q�b�g(���)�̃N���A
	float clearColor[] = { 0.1f,0.25f, 0.5f,0.0f };//���ۂ��F
	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// �r���[�|�[�g�̐ݒ�
	CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, Application::window_width, Application::window_height);
	_cmdList->RSSetViewports(1, &viewport);
	// �V�U�����O��`�̐ݒ�
	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, Application::window_width, Application::window_height);
	_cmdList->RSSetScissorRects(1, &rect);
}

void Wrapper::PreRunShadow()
{	//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
	auto bbIdx = SwapChain()->GetCurrentBackBufferIndex();

	////���\�[�X�o���A�iPresent�̃����_�^�[�Q�b�g�j��Ԃ̕ύX
	//CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffer[bbIdx].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//_cmdList->ResourceBarrier(1, &barrier);

	//�`�����w��
	auto handle = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += Singleton_Heap::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV
	);
	_cmdList->OMSetRenderTargets(0, nullptr, false, &handle);

	// �r���[�|�[�g�̐ݒ�
	CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, Application::window_width, Application::window_height);
	_cmdList->RSSetViewports(1, &viewport);
	// �V�U�����O��`�̐ݒ�
	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, Application::window_width, Application::window_height);
	_cmdList->RSSetScissorRects(1, &rect);
}

void Wrapper::PostRun() {
	//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();

	//���\�[�X�o���A�i�����_�^�[�Q�b�g��Present�j��Ԃ̕ύX
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffer[bbIdx].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	_cmdList->ResourceBarrier(1, &barrier);

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
		if (event == 0) { assert(0); }

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
	//auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgifactory));
	return S_OK;
}

HRESULT Wrapper::InitializeDevice() {
	//	auto _result = result();
	//	//DXGI�t�@�N�g���[�̐���
	//	D3D_FEATURE_LEVEL levels[] = {
	//		D3D_FEATURE_LEVEL_12_1,
	//		D3D_FEATURE_LEVEL_12_0,
	//		D3D_FEATURE_LEVEL_11_1,
	//		D3D_FEATURE_LEVEL_11_0
	//	};
	//
	//	//�A�_�v�^�[�񋓗p
	//	vector<ComPtr<IDXGIAdapter>>adapters;
	//	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	//	ComPtr<IDXGIAdapter> tmpAdapter = nullptr;
	//	//�A�_�v�^�[
	//	for (int i = 0; _dxgifactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
	//		adapters.push_back(tmpAdapter);
	//	}
	//
	//	for (auto adpt : adapters) {
	//		DXGI_ADAPTER_DESC adesc = {};
	//		adpt->GetDesc(&adesc);//�A�_�v�^�[�̐����I�u�W�F�N�g�擾
	//		wstring strDesc = adesc.Description;
	//		//�T�������A�_�v�^�[�̖��O���m�F
	//		if (strDesc.find(L"NVIDIA") != string::npos) {
	//			tmpAdapter = adpt;
	//			break;
	//		}
	//	}
	//
	//	//Direct3D�f�o�C�X�̐ݒ�
	//	D3D_FEATURE_LEVEL featureLevel;
	//	//�t�B�[�`���[���x���̎擾
	//	for (auto lv : levels) {
	//		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK) {
	//			featureLevel = lv;
	//			_result = S_OK;
	//			break;
	//		}
	//	}
	//
	//#ifdef _DEBUG
	//	ComPtr<ID3D12InfoQueue> infoQueue;
	//	if (SUCCEEDED(_dev->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
	//		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	//		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	//	}
	//
	//#endif // _DEBUG
	//
	return S_OK;
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

	auto result = Singleton_Heap::GetInstance()->GetDXGIFactory()->CreateSwapChainForHwnd(
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
	auto result = Singleton_Heap::GetInstance()->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	result = Singleton_Heap::GetInstance()->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));
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
	result = Singleton_Heap::GetInstance()->GetDevice()->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));

	_cmdQueue->SetName(L"queue");

	return result;
}

HRESULT Wrapper::InitializeRenderHeap()
{
	//�f�B�X�N���v�^�q�[�v����(�����_)
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//�ǂ�ȃr���[�����̂�(����̓����_�^�[�Q�b�g�Ȃ̂�RTV)
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;//�\����2��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//���Ɏw��Ȃ�

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	auto result = Singleton_Heap::GetInstance()->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_rtvHeaps.ReleaseAndGetAddressOf()));
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

	for (uint32_t i = 0; i < swcDesc.BufferCount; ++i) {
		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffer[i]));
		if (FAILED(result)) {
			assert(0);
			return result;
		}

		_backBuffer[i]->SetName(L"buffer");

		//rtvDesc.Format = _backBuffer[i]->GetDesc().Format;
		//�����_�^�[�Q�b�g�r���[�̐���
		Singleton_Heap::GetInstance()->GetDevice()->CreateRenderTargetView(_backBuffer[i].Get(), &rtvDesc, handle);
		//�|�C���^�[�����炷
		handle.ptr += Singleton_Heap::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	return result;
}

HRESULT Wrapper::InitializeDescHeap() {
	//�f�B�X�N���v�^�q�[�v����(�ėp)
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�ǂ�ȃr���[�����̂�()
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1024;//�S�̂̃q�[�v�̈搔(1�F�V�[���A2�F�[�x�e�N�X�`���A3�`�FPMD�}�e���A��)
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//���Ɏw��Ȃ�

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	auto result = Singleton_Heap::GetInstance()->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_descHeap.ReleaseAndGetAddressOf()));
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
	heapHandle_CPU = Singleton_Heap::GetInstance()->GetDescHeap()->GetCPUDescriptorHandleForHeapStart();
	heapHandle_GPU = Singleton_Heap::GetInstance()->GetDescHeap()->GetGPUDescriptorHandleForHeapStart();
	//_backBuffer.resize(swcDesc.BufferCount);

	////SRGB�����_�^�[�Q�b�g�r���[�ݒ�
	//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	//rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//for (int i = 0; i < swcDesc.BufferCount; ++i) {
	//	result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffer[i]));
	//	if (FAILED(result)) {
	//		assert(0);
	//		return result;
	//	}

	//	_backBuffer[i]->SetName(L"buffer");

	//	//rtvDesc.Format = _backBuffer[i]->GetDesc().Format;
	//	//�����_�^�[�Q�b�g�r���[�̐���
	//	_dev->CreateRenderTargetView(_backBuffer[i].Get(), &rtvDesc, handle);
	//	//�|�C���^�[�����炷
	//	handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//}

	return result;
}

HRESULT Wrapper::InitializeDepthBuff(SIZE ret) {

	//�[�x�e�N�X�`���p
	D3D12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, ret.cx, ret.cy);
	resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//�ʏ�̐[�x�o�b�t�@
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

	auto result = Singleton_Heap::GetInstance()->GetDevice()->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//�[�x�������ݗp
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	resdesc.Width = shadow_difinition;
	resdesc.Height = shadow_difinition;
	result = Singleton_Heap::GetInstance()->GetDevice()->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//�[�x�������ݗp
		&depthClearValue,
		IID_PPV_ARGS(_lightDepthBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�[�x�̂��߂̃f�B�X�N���v�^�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 2;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = Singleton_Heap::GetInstance()->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�[�x�r���[����
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	auto handle = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
	Singleton_Heap::GetInstance()->GetDevice()->CreateDepthStencilView(
		_depthBuffer.Get(),
		&dsvDesc,
		handle);

	handle.ptr += Singleton_Heap::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	Singleton_Heap::GetInstance()->GetDevice()->CreateDepthStencilView(
		_lightDepthBuffer.Get(),
		&dsvDesc,
		handle);

	D3D12_SHADER_RESOURCE_VIEW_DESC texResdesc = {};
	texResdesc.Format = DXGI_FORMAT_R32_FLOAT;
	texResdesc.Texture2D.MipLevels = 1;
	texResdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	texResdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

	//�[�x�e�N�X�`�����\�[�X���쐬
	Singleton_Heap::GetInstance()->GetDevice()->CreateShaderResourceView(
		_depthBuffer.Get(), &texResdesc, heapHandle_CPU);

	heapHandle_CPU.ptr += Singleton_Heap::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//���C�g�f�v�X
	Singleton_Heap::GetInstance()->GetDevice()->CreateShaderResourceView(
		_lightDepthBuffer.Get(), &texResdesc, heapHandle_CPU);

	return result;
}

HRESULT Wrapper::InitializeFence() {
	//�t�F���X����
	auto result = Singleton_Heap::GetInstance()->GetDevice()->CreateFence(_fenceval, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf()));

	return result;
}

HRESULT Wrapper::CreateSceneView()
{
	HRESULT result;

	CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0_1) + 0xff) & ~0xff);
	result = Singleton_Heap::GetInstance()->GetDevice()->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_sceneConstBuff.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(SUCCEEDED(result));
		return result;
	}

	_mappedSceneData = nullptr;//�}�b�v��������|�C���^
	result = _sceneConstBuff->Map(0, nullptr, (void**)&_mappedSceneData);//�}�b�v

	const XMMATRIX& matViewProjection = matView * matProjection;
	const XMFLOAT3& cameraPos = eye;

	const XMFLOAT4 planeVec(0, 1, 0, 0);
	const XMFLOAT3 lightVec(1, -1, 1);

	_mappedSceneData->viewproj = matViewProjection;
	_mappedSceneData->shadow = XMMatrixShadow(
		XMLoadFloat4(&planeVec),
		-XMLoadFloat3(&lightVec));
	_mappedSceneData->cameraPos = cameraPos;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _sceneConstBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = UINT(_sceneConstBuff->GetDesc().Width);
	//�萔�o�b�t�@�r���[�̍쐬
	Singleton_Heap::GetInstance()->GetDevice()->CreateConstantBufferView(&cbvDesc, heapHandle_CPU);

	auto inc = Singleton_Heap::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	heapHandle_CPU.ptr += inc;
	return result;
}

void Wrapper::ClearDepthShadow()
{
	// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	dsvH.ptr += Singleton_Heap::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// �[�x�o�b�t�@�̃N���A
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
ComPtr<ID3D12DescriptorHeap> Wrapper::CreateDescriptorHeapForImgui()
{
	ComPtr<ID3D12DescriptorHeap> ret; //imgui�p�q�[�v

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	Singleton_Heap::GetInstance()->GetDevice()->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));

	return ret;
}

void Wrapper::SceneUpdate()
{
	//HRESULT result;
	//_mappedSceneData = nullptr;//�}�b�v��������|�C���^
	//result = _sceneConstBuff->Map(0, nullptr, (void**)&_mappedSceneData);//�}�b�v

	//const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	//const XMFLOAT3& cameraPos = camera->GetEye();
	//const XMFLOAT4 planeVec(0, 1, 0, 0);
	//const XMFLOAT4 light(1, -1, 1, 0);
	////���C�g�����x�N�g��
	//XMVECTOR lightVec = XMLoadFloat4(&light);

	//XMVECTOR eye = XMLoadFloat3(&cameraPos);
	//XMVECTOR terget = XMLoadFloat3(&camera->GetTarget());
	//XMVECTOR up = XMLoadFloat3(&camera->GetUp());

	////�J�������璍���_�܂ł̃x�N�g�������C�g�����x�N�g���ɏ�Z���ă��C�g�ʒu������
	//auto lightPos = terget + XMVector3Normalize(lightVec) * XMVector3Length(XMVectorSubtract(terget, eye)).m128_f32[0];

	////�ʏ�J�����̃r���[�s��
	//_mappedSceneData->viewproj = matViewProjection;
	////���C�g��������̃r���[�s��
	//_mappedSceneData->lightCamera = XMMatrixLookAtLH(lightPos, terget, up) * XMMatrixOrthographicLH(40, 40, 1.0f, 100.0f);

	//_mappedSceneData->shadow = XMMatrixShadow(
	//	XMLoadFloat4(&planeVec),
	//	-lightVec);

	//_mappedSceneData->cameraPos = cameraPos;

	//_sceneConstBuff->Unmap(0, nullptr);
}

ComPtr<IDXGISwapChain4> Wrapper::SwapChain() {
	return _swapchain;
}

ComPtr<ID3D12GraphicsCommandList> Wrapper::CommandList() {
	return _cmdList;
}

void Wrapper::DrawLight(ID3D12GraphicsCommandList* cmdlist)
{
	assert(cmdlist);
	light->Draw(cmdlist, 3);
}

void Wrapper::DrawDepth()
{
	//�[�xSRV
	ID3D12DescriptorHeap* sceneheaps[] = { Singleton_Heap::GetInstance()->GetDescHeap() };
	_cmdList->SetDescriptorHeaps(1, sceneheaps);
	heapHandle_GPU = Singleton_Heap::GetInstance()->GetDescHeap()->GetGPUDescriptorHandleForHeapStart();
	heapHandle_GPU.ptr += Singleton_Heap::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;

	_cmdList->SetGraphicsRootDescriptorTable(4, heapHandle_GPU);

	////�[�xSRV
	//_cmdList->SetDescriptorHeaps(1, _descHeap.GetAddressOf());
	//heapHandle_GPU = _descHeap->GetGPUDescriptorHandleForHeapStart();
	//heapHandle_GPU.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;

	//_cmdList->SetGraphicsRootDescriptorTable(4, heapHandle_GPU);
}

void Wrapper::SceneDraw()
{
	//���݂̃V�[��(�r���[�v���W�F�N�V����)���Z�b�g
	ID3D12DescriptorHeap* sceneheaps[] = { Singleton_Heap::GetInstance()->GetDescHeap() };
	_cmdList->SetDescriptorHeaps(1, sceneheaps);
	_cmdList->SetGraphicsRootDescriptorTable(0, Singleton_Heap::GetInstance()->GetDescHeap()->GetGPUDescriptorHandleForHeapStart());

	////���݂̃V�[��(�r���[�v���W�F�N�V����)���Z�b�g
	//ID3D12DescriptorHeap* sceneheaps[] = { _descHeap.Get() };
	//_cmdList->SetDescriptorHeaps(1, sceneheaps);
	//_cmdList->SetGraphicsRootDescriptorTable(0, _descHeap->GetGPUDescriptorHandleForHeapStart());
}