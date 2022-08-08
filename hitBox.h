#pragma once
#include "..\object\object3D.h"
#include "..\Collision\Collision.h"
#include "..\PMD\PMDmodel.h"

class Wrapper;
class PMDmodel;
class HitBox : public Object3Ds {

	static Wrapper* dx12;
	//PMDmodel* model{};

	struct Transform {
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
		XMMATRIX bones[256];
	};

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0 {
		XMMATRIX viewproj;
		XMFLOAT3 cameraPos;
	};

	static std::vector<HitBox*> hitBox;

	XMMATRIX* _mappedMatrices = nullptr;
	ComPtr<ID3D12Resource> transformBuff = nullptr;
	std::vector<DirectX::XMMATRIX> boneMatrices;


public:
	static std::vector<Sqhere> _hit;

	static HRESULT CreateTransform();

public:
	HitBox();
	static HitBox* Create();
	void Update(std::vector<DirectX::XMMATRIX> _boneMatrices, XMFLOAT3 rot);
	void Draw();
	static void mainDraw();
	static void mainUpdate(std::vector<DirectX::XMMATRIX> _boneMatrices, XMFLOAT3 rot);

	static void CreateHitBox(XMFLOAT3 pos, Model* model);
	static void CreatePipeline(Wrapper* _dx12);

	static std::vector<HitBox*> GetHit() { return hitBox; }
};