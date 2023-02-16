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
	//namespace�ȗ�
	//�G�C���A�X
	template <class T> using Comptr = Microsoft::WRL::ComPtr<T>;

private:
	//�E�B���h�E�Y�A�v���P�[�V����
	//Application* WinApp;

		// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0_1
	{
		//XMFLOAT4 color;	// �F (RGBA)
		//XMMATRIX mat;	// �R�c�ϊ��s��
		XMMATRIX viewproj;
		XMMATRIX shadow;
		XMFLOAT3 cameraPos;
	};

	//dx12���
	ComPtr<ID3D12Device>				_dev = nullptr;				//�f�o�C�X
	ComPtr<IDXGIFactory6>				_dxgifactory = nullptr;		//dxgi�t�@�N�g���[
	//�X���b�v�`�F�[��
	ComPtr<IDXGISwapChain4>				_swapchain = nullptr;		//�X���b�v�`�F�[��
	//�R�}���h
	ComPtr<ID3D12CommandAllocator>		_cmdAllocator = nullptr;	// �R�}���h���X�g�A�P���[�^�[
	ComPtr<ID3D12GraphicsCommandList>	_cmdList = nullptr;			//�R�}���h���X�g
	ComPtr<ID3D12CommandQueue>			_cmdQueue = nullptr;		//�R�}���h�L��
	//�t�F���X
	ComPtr<ID3D12Fence>					_fence = nullptr;			//�t�F���X
	UINT								_fenceval = 0;
	//�\���֘A
	vector<ComPtr<ID3D12Resource>>      _backBuffer;
	ComPtr<ID3D12DescriptorHeap>		_rtvHeaps = nullptr;		//�����_�q�[�v
	ComPtr<ID3D12DescriptorHeap>		_descHeap = nullptr;		//�ėp�q�[�v
	D3D12_CPU_DESCRIPTOR_HANDLE			heapHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			_heapHandle;
	//�[�x�o�b�t�@
	ComPtr<ID3D12DescriptorHeap>		_dsvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap>		_depthSRVHaep = nullptr;	//�[�x�e�N�X�`��
	ComPtr<ID3D12Resource>				_depthBuffer = nullptr;
	//�V�[�����\������o�b�t�@�܂��
	ConstBufferDataB0_1*				_mappedSceneData;
	ComPtr<ID3D12Resource>				_sceneConstBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap>		_sceneDescHeap = nullptr;
	//imgui
	ComPtr<ID3D12DescriptorHeap>		_heapForImgui; //�q�[�v�ێ��p

	D3D12_DESCRIPTOR_HEAP_DESC          heapDesc = {};

	static Light* light;

	static Camera* camera;
	// �r���[�s��
	static XMMATRIX matView;
	// �ˉe�s��
	static XMMATRIX matProjection;
	// ���_���W
	static XMFLOAT3 eye;
	// �����_���W
	static XMFLOAT3 target;
	// ������x�N�g��
	static XMFLOAT3 up;
	//�J����������
	static void InitalizeCamera(int window_width, int window_height) {
		// �r���[�s��̐���
		matView = XMMatrixLookAtLH(
			XMLoadFloat3(&eye),
			XMLoadFloat3(&target),
			XMLoadFloat3(&up));

		// ���s���e�ɂ��ˉe�s��̐���
		//constMap->mat = XMMatrixOrthographicOffCenterLH(
		//	0, window_width,
		//	window_height, 0,
		//	0, 1);
		// �������e�ɂ��ˉe�s��̐���
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

	//�㏈��
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
		// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		// �[�x�o�b�t�@�̃N���A
		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="camera">�J����</param>
	static void SetCamera(Camera* camera) {
		Wrapper::camera = camera;
	}

	static void SetLight(Light* light) {
		Wrapper::light = light;
	}

	void DrawLight(ID3D12GraphicsCommandList* cmdlist);

	void SceneDraw() {
		//���݂̃V�[��(�r���[�v���W�F�N�V����)���Z�b�g
		ID3D12DescriptorHeap* sceneheaps[] = { _descHeap.Get() };
		_cmdList->SetDescriptorHeaps(1, sceneheaps);
		_cmdList->SetGraphicsRootDescriptorTable(0, _descHeap->GetGPUDescriptorHandleForHeapStart());
	}
	void SceneUpdate() {
		HRESULT result;
		_mappedSceneData = nullptr;//�}�b�v��������|�C���^
		result = _sceneConstBuff->Map(0, nullptr, (void**)&_mappedSceneData);//�}�b�v

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