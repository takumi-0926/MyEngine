#pragma once
#include <DirectXMath.h>
#include "includes.h"

#include "dx12Wrapper.h"
#include "pipelineSet.h"
#include "PMDModel.h"

#include "baseObject.h"

class Wrapper;
class PMDmodel;
class PMDobject : public BaseObject {
	friend PMDmodel;
	friend Wrapper;
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

	//3Dオブジェクト用
	static ComPtr<ID3D12RootSignature>			_rootsignature;
	static ComPtr<ID3D12PipelineState>			_pipelinestate;

	//シーンを構成するバッファまわり
	ConstBufferDataB0_1* _mappedSceneData;
	ComPtr<ID3D12Resource> _sceneConstBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap> _sceneDescHeap = nullptr;

	//パイプライン初期化
	HRESULT CreateGraphicsPipelinePMD();
	//ルートシグネチャ初期化
	HRESULT CreateRootSignaturePMD();

	HRESULT CreateSceneView();

	HRESULT CreateDescHeap();

public:
	PMDobject();
	~PMDobject();

	//void SetPosition(XMFLOAT3 position) {
	//	this->position = position;
	//}
public:
	//更新
	void Update();
	//描画
	void Draw();

	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return cmdList; }
private:
	//ComPtr<ID3D12Resource> PMDconstBuffB0;

public:
	//// 色
	//XMFLOAT4 color = { 1,1,1,1 };
	//// ローカルスケール
	//XMFLOAT3 scale = { 1,1,1 };
	//// X,Y,Z軸回りのローカル回転角
	//XMFLOAT3 rotation = { 0,0,0 };
	//// ローカル座標
	//XMFLOAT3 position = { 0,0,0 };
	//// ローカルワールド変換行列
	//XMMATRIX matWorld;

	//PMDmodel* model = nullptr;
};
