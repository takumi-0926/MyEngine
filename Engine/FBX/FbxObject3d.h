#pragma once

#include "FbxModel.h"
#include "Camera\Camera.h"
#include "Collision\BaseCollision.h"
#include "Collision\CollisionManager.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

enum MotionType {
	WalkMotion = 0,
	AttackMotion,
};

class FbxObject3d {
protected:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	struct ConstBufferDetaTransform {
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
	};

	static const int MAX_BONES = 256;

	struct ConstBufferDataSkin {
		XMMATRIX bones[MAX_BONES];
	};

	//�A�j���[�V�����ۑ��p�\����
	struct AnimationInfelno {
		const char* Name;	//���O
		FbxTime startTime;	//�J�n����
		FbxTime endTime;	//�I������
		FbxTakeInfo* info;	//
		FbxAnimStack* stack;//
	};
	vector<AnimationInfelno> animas;

public:
	virtual void Initialize();

	virtual void Update();

	virtual void Draw(ID3D12GraphicsCommandList* cmdList);

	//�s��̍X�V
	void UpdateWorldMatrix();

	void SetModel(FbxModel* model) {
		this->model = model;
		LoadAnima();
	}

	void PlayAnimation(int playNum);
	void LoadAnima();

public:
	static void SetDevice(ID3D12Device* device) { FbxObject3d::device = device; }
	static void SetCamera(Camera* camera) {
		FbxObject3d::camera = camera;
	}
	void SetPosition(XMFLOAT3 pos) { this->position = pos; }
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }
	static void CreateGraphicsPipeline();

	void SetCollider(BaseCollider* collider);
	virtual void OnCollision(const CollisionInfo& info) {}

	/// <summary>
	/// ���[���h�s��擾
	/// </summary>
	/// <returns></returns>
	const XMMATRIX GetMatWorld() { return matWorld; }

private:
	static ID3D12Device* device;

	static Camera* camera;

	static ComPtr<ID3D12RootSignature> rootsignature;

	static ComPtr<ID3D12PipelineState> pipelinestate;

protected:
	XMFLOAT3 scale = { 1,1,1 };

	XMFLOAT3 rotation = { 0,0,0 };

	XMFLOAT3 position = { 0,0,0 };

	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matScale = {};
	XMMATRIX matRot = {};
	XMMATRIX matTrans = {};
	XMMATRIX matWorld = {};

	FbxModel* model = nullptr;

	FbxTime frameTime;

	//FbxTime startTime;	//�J�n����
	//FbxTime endTime;	//�I������

	FbxTime currentTime;

	bool isPlay = false;

	bool changePlay = false;

	int nowPlayMotion = MotionType::WalkMotion;

	ComPtr<ID3D12Resource> constBufferTransform;

	ComPtr<ID3D12Resource> constBuffSkin;

	//��]�s��̎g�p��
	bool useRotMat = false;

	BaseCollider* collider = nullptr;

public:
	XMFLOAT3 GetPosition() { return position; }
};