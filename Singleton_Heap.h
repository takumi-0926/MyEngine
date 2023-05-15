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
	//エイリアス
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

	ComPtr<ID3D12Device>		   device = nullptr;//デバイス
	ComPtr<IDXGIFactory6>	 _dxgifactory = nullptr;//dxgiファクトリー
	ComPtr<ID3D12DescriptorHeap> descHeap = nullptr;//汎用ヒープ
	/// <summary>
	/// 0 : シーン
	/// 1 : 通常深度
	/// 2 : ライト深度
	/// 3 : imGui
	/// 4 : ポストエフェクト(SRV)
	/// 5 : ポストエフェクト(深度SRV)
	/// 6 : 
	/// 100～ : オブジェクト3D
	/// 200～ : FBX
	/// 300～ : スプライト
	/// </summary>


	UINT descriptorHandleIncrementSize = 0;

public:
	//対応先ハンドル番号
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
	//ディスクリプタヒープ生成(汎用)
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//どんなビューを作るのか()
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1024;//全体のヒープ領域数(1：シーン、2：深度テクスチャ、3～：PMDマテリアル)
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//特に指定なし

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
