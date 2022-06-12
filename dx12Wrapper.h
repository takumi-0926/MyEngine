#pragma once
#include "includes.h"
#include "application.h"

class Wrapper {
public:
	//namespace�ȗ�
	//�G�C���A�X
	template <class T> using Comptr = Microsoft::WRL::ComPtr<T>;

private:
	//�E�B���h�E�Y�A�v���P�[�V����
	//Application* WinApp;
	
	//dx12���
	ComPtr<ID3D12Device>				_dev		 = nullptr;		//�f�o�C�X
	ComPtr<IDXGIFactory6>				_dxgifactory = nullptr;		//dxgi�t�@�N�g���[
	//�X���b�v�`�F�[��
	ComPtr<IDXGISwapChain4>				_swapchain = nullptr;		//�X���b�v�`�F�[��
	//�R�}���h
	ComPtr<ID3D12CommandAllocator>		_cmdAllocator = nullptr;	// �R�}���h���X�g�A�P���[�^�[
	ComPtr<ID3D12GraphicsCommandList>	_cmdList	  = nullptr;	//�R�}���h���X�g
	ComPtr<ID3D12CommandQueue>			_cmdQueue	  = nullptr;	//�R�}���h�L��
	//�t�F���X
	ComPtr<ID3D12Fence>					_fence	  = nullptr;		//�t�F���X
	UINT								_fenceval = 0;
	//�\���֘A
	vector<ComPtr<ID3D12Resource>>      _backBuffer;
	ComPtr<ID3D12DescriptorHeap>		_rtvHeaps	   = nullptr;	//�����_�q�[�v
	ComPtr<ID3D12DescriptorHeap>		_basicDescHeap = nullptr;	//�ėp�q�[�v
	D3D12_CPU_DESCRIPTOR_HANDLE			heapHandle;
	//�[�x�o�b�t�@
	ComPtr<ID3D12DescriptorHeap>		_dsvHeap	 = nullptr;
	ComPtr<ID3D12Resource>				_depthBuffer = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC          heapDesc = {};
public:
	~Wrapper();

	bool Init(HWND _hwnd,SIZE _ret);

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
	
	HRESULT InitializeDepthBuff(SIZE ret);
	
	HRESULT InitializeFence();
	
	void ClearDepthBuffer() {
		// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		// �[�x�o�b�t�@�̃N���A
		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	}

	//ComPtr<ID3D12Device> Device();
	ID3D12Device* GetDevice() { return _dev.Get(); }
	ComPtr<IDXGISwapChain4> SwapChain();
	ComPtr<ID3D12GraphicsCommandList> CommandList();
	ComPtr<ID3D12DescriptorHeap> GetDescHeap() { return _basicDescHeap.Get(); }
};