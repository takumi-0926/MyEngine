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
	static Wrapper* dx12;
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

private:
	//パイプライン初期化
	HRESULT CreateGraphicsPipelinePMD();
	//ルートシグネチャ初期化
	HRESULT CreateRootSignaturePMD();

public:
	PMDobject(/*Wrapper* dx12*/);
	~PMDobject();

	PMDmodel* model = nullptr;
public:
	static bool StaticInitialize(Wrapper* _dx12);
	static PMDobject* Create(PMDmodel* _model = nullptr);

	bool Initialize(PMDmodel* _model);
	//更新
	void Update();
	void UpdateWorldMatrix();
	//描画
	void Draw();

	void SetModel(PMDmodel* _model);
	void SetScale(XMFLOAT3 _scale)  { this->scale =  _scale; }
	void SetPosition(XMFLOAT3 _pos) { this->position = _pos; }
private:
	ComPtr<ID3D12Resource> PMDconstBuffB1; // 定数バッファ

	// 色
	XMFLOAT4 color = { 1,1,1,1 };
	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	// ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	// ローカルワールド変換行列
	XMMATRIX matWorld;

	XMMATRIX matRot = XMMatrixIdentity();;

};
