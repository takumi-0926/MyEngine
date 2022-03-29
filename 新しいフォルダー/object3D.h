#pragma once
#include <DirectXMath.h>
#include "includes.h"

#include"LoadHlsl.h"

////3Dオブジェクト用頂点データ
//struct  Vertex {
//	DirectX::XMFLOAT3 pos;
//	DirectX::XMFLOAT3 normal;
//	DirectX::XMFLOAT2 uv;
//};
//
////3Dオブジェクト構造体
//struct Object3D {
//	ComPtr<ID3D12Resource> constBuff = nullptr;
//	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
//	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
//
//	//アフィン変換行列
//	XMFLOAT3 scale = { 1,1,1 };//スケーリング倍率
//	XMFLOAT3 rotation = { 0,0,0 };//回転角
//	XMFLOAT3 position = { 0,0,0 };//座標
//
//	XMMATRIX worldMat;
//
//	Object3D* parent = nullptr;
//
//	bool Flag;
//};
//
////シェーダに渡すための行列データ
//struct constBufferData {
//	XMFLOAT4 color;//色(RGBA)
//	XMMATRIX mat;
//};
//
//const int constantBufferNum = 128;
//
//class Object3Ds : LoadHlsls
//{
//private:
//	ID3DBlob* _vsBlob = nullptr; //頂点シェーダ用
//	ID3DBlob* _psBlob = nullptr; //ピクセルシェーダ用
//	ID3DBlob* _gsBlob = nullptr; //ジオメトリシェーダ用
//
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
//
//	ComPtr<ID3D12RootSignature>			_rootsignature;
//	ComPtr<ID3DBlob>					_rootSigBlob = nullptr;
//	ComPtr<ID3D12PipelineState>			_pipelinestate = nullptr;
//
//
//public:
//	//パイプライン生成
//	PipelineSet CreatePipeline(ID3D12Device* _dev);
//
//	//3Dオブジェクト生成
//	void CreateObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap);
//
//	//3Dオブジェクトの初期化
//	void InitializeObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap);
//	//3Dオブジェクトの更新
//	void UpdateObject3D(Object3D* object, XMMATRIX& viewMat, XMMATRIX& projMat);
//	//3Dオブジェクトの描画
//	void DrawObject3D(Object3D* object, ID3D12GraphicsCommandList* _cmdList,
//		ID3D12DescriptorHeap* descHeap, D3D12_VERTEX_BUFFER_VIEW& vbView,
//		D3D12_INDEX_BUFFER_VIEW& ibView, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV, UINT numIndices);
//};