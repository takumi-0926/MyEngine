#pragma once
#include <DirectXMath.h>
#include "..\includes.h"

#include "PMDModel.h"
#include "..\Application\dx12Wrapper.h"
#include "..\object\baseObject.h"

class Wrapper;
class PMDmodel;
class PMDobject : public BaseObject {
	friend PMDmodel;
	Wrapper* dx12;
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	//std::省略
	using string = std::string;

	struct Transform {
		void* operator new(size_t size);
		XMMATRIX world;
	};

	//3Dオブジェクト用
	static ComPtr<ID3D12RootSignature>	_rootsignature;
	static ComPtr<ID3D12PipelineState>	_pipelinestate;

	// デスクリプタサイズ
	UINT descriptorHandleIncrementSize;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap = nullptr;

	////座標変換
	//Transform transform;
	//Transform* mappedTransform = nullptr;
	//ComPtr<ID3D12Resource> transformBuff = nullptr;
	//ComPtr<ID3D12Resource> transformMat = nullptr;
	//ComPtr<ID3D12DescriptorHeap> transformHeap = nullptr;

	//シーンを構成するバッファまわり
	//ConstBufferDataB0_1* _mappedSceneData;
	//ComPtr<ID3D12Resource> _sceneConstBuff = nullptr;
	//ComPtr<ID3D12DescriptorHeap> _sceneDescHeap = nullptr;

	//パイプライン初期化
	HRESULT CreateGraphicsPipelinePMD();
	//ルートシグネチャ初期化
	HRESULT CreateRootSignaturePMD();

	//HRESULT CreateSceneView();

public:
	PMDobject(Wrapper* dx12);
	~PMDobject();

	PMDmodel model;
public:
	//更新
	void Update();
	//描画
	void Draw();

	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return cmdList; }
};
