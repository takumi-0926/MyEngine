#pragma once

#include <cassert>
#include "d3dx12.h"
#include <dxgi.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace std;

class Singleton_Heap
{
	//�G�C���A�X
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Singleton_Heap(const Singleton_Heap& heap) = delete;
	Singleton_Heap& operator=(const Singleton_Heap& heap) = delete;

	static Singleton_Heap* GetInstance();

	HRESULT CreateDescHeap();
	HRESULT CreateDevice();
	HRESULT result();

	ID3D12DescriptorHeap* GetDescHeap();
	ID3D12Device* GetDevice();
	UINT GetDescriptorIncrementSize();

private:
	Singleton_Heap() = default;
	~Singleton_Heap() {};

	ComPtr<ID3D12Device>		   device = nullptr;//�f�o�C�X
	ComPtr<IDXGIFactory6>	 _dxgifactory = nullptr;//dxgi�t�@�N�g���[
	ComPtr<ID3D12DescriptorHeap> descHeap = nullptr;//�ėp�q�[�v
	/// <summary>
	/// 0 : �V�[��
	/// 1 : �ʏ�[�x
	/// 2 : ���C�g�[�x
	/// 3 : imGui
	/// 4 : �|�X�g�G�t�F�N�g(SRV)
	/// 5 : �|�X�g�G�t�F�N�g(�[�xSRV)
	/// 6 : 
	/// 100�` : �I�u�W�F�N�g3D
	/// 200�` : FBX
	/// 300�` : �X�v���C�g
	/// </summary>


	UINT descriptorHandleIncrementSize = 0;

public:
	//�Ή���n���h���ԍ�
	int Object3DTexture = 100;
	int FbxTexture	  = 200;
	int SpriteTexture	  = 300;

};

inline Singleton_Heap *Singleton_Heap::GetInstance()
{
	static Singleton_Heap instance;
	return &instance;
}

inline HRESULT Singleton_Heap::CreateDescHeap()
{
	//�f�B�X�N���v�^�q�[�v����(�ėp)
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�ǂ�ȃr���[�����̂�()
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1024;//�S�̂̃q�[�v�̈搔(1�F�V�[���A2�F�[�x�e�N�X�`���A3�`�FPMD�}�e���A��)
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//���Ɏw��Ȃ�

	auto result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(descHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return result;
}

inline HRESULT Singleton_Heap::CreateDevice()
{
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
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&device)) == S_OK) {
			featureLevel = lv;
			_result = S_OK;
			break;
		}
	}

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	}

#endif // _DEBUG

	return _result;
}

inline HRESULT Singleton_Heap::result()
{
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgifactory));
	return result;
}

inline ID3D12DescriptorHeap* Singleton_Heap::GetDescHeap()
{
	return descHeap.Get();
}

inline ID3D12Device* Singleton_Heap::GetDevice()
{
	return device.Get();
}

inline UINT Singleton_Heap::GetDescriptorIncrementSize()
{
	return descriptorHandleIncrementSize;
}
