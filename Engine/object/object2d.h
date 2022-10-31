#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

//class object2d
//{
//	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
//
//	using XMFLOAT2 = DirectX::XMFLOAT2;
//	using XMFLOAT3 = DirectX::XMFLOAT3;
//	using XMFLOAT4 = DirectX::XMFLOAT4;
//	using XMMATRIX = DirectX::XMMATRIX;
//
//	//std::省略
//	using string = std::string;
//
//	static ComPtr<ID3D12Device> device;
//	// 頂点バッファ
//	ComPtr<ID3D12Resource> vertBuff;
//	// インデックスバッファ
//	ComPtr<ID3D12Resource> indexBuff;
//
//	//オブジェクト用
//	ComPtr<ID3D12RootSignature>	_rootsignature;
//	ComPtr<ID3D12PipelineState>	_pipelinestate;
//
//	// 頂点バッファビュー
//	D3D12_VERTEX_BUFFER_VIEW vbView;
//	D3D12_INDEX_BUFFER_VIEW ibView;
//
//	bool initFlag = false;
//
//public:
//	static bool StaticInitialize(ID3D12Device* _device);
//
//	static object2d* Create();
//
//	void CreateBuffer(XMFLOAT3 _vertices[], unsigned short _indices[]);
//
//	void CreateTryangle();
//
//	void CreateLine(float x1, float y1, float x2, float y2);
//
//	void CreateCircle(float x, float y, float r);
//
//	void DarwLine(ID3D12GraphicsCommandList* cmdList);
//};

