#pragma once
#include "Collision\Collision.h"
#include <d3d12.h>

class Wrapper;

class HitSphere{

	static Wrapper* dx12;

	struct Transform {
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
	};

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0 {
		XMMATRIX viewproj;
		XMFLOAT3 cameraPos;
	};

	//ComPtr<ID3D12Resource> transformBuff = nullptr;

private:
	static HitSphere* Create();
	static void CreatePipeline();
public:
	HitSphere();
	void Update();
	void Draw();

};