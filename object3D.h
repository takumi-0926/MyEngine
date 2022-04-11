#pragma once
#include <DirectXMath.h>
#include "includes.h"

#include "pipelineSet.h"
#include "Model.h"
#include "PMDmodel.h"
#include "Camera.h"

#include "baseObject.h"

//3D�I�u�W�F�N�g�p���_�f�[�^
struct  Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

//3D�I�u�W�F�N�g�\����
//struct Object3D {
//	ComPtr<ID3D12Resource> constBuff = nullptr;
//	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
//	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
//
//	//�A�t�B���ϊ��s��
//	XMFLOAT3 scale = { 1,1,1 };//�X�P�[�����O�{��
//	XMFLOAT3 rotation = { 0,0,0 };//��]�p
//	XMFLOAT3 position = { 0,0,0 };//���W
//
//	XMMATRIX worldMat;
//
//	Object3D* parent = nullptr;
//
//	bool Flag;
//};

////�V�F�[�_�ɓn�����߂̍s��f�[�^
//struct constBufferData {
//	XMFLOAT4 color;//�F(RGBA)
//	XMMATRIX mat;
//};
//
//const int constantBufferNum = 128;

class Object3Ds : public BaseObject
{
private: // �ÓI�����o�ϐ�
	//3D�I�u�W�F�N�g�p
	static ComPtr<ID3D12RootSignature>			_rootsignature;
	//static ComPtr<ID3DBlob>						_rootSigBlob;
	static ComPtr<ID3D12PipelineState>			_pipelinestate;

public:
	// �ÓI������
	/// <param name="device">�f�o�C�X</param>
	/// <param name="window_width">��ʕ�</param>
	/// <param name="window_height">��ʍ���</param>
	/// <returns>����</returns>
	static bool StaticInitialize(ID3D12Device* device, SIZE ret);

	// 3D�I�u�W�F�N�g����
	static Object3Ds* Create();

	//static void InitalizeCamera(int window_width, int window_height);

	/// <summary>
	/// �O���t�B�b�N�p�C�v���C������
	/// </summary>
	/// <returns>����</returns>
	static bool InitializeGraphicsPipeline();

	///// <summary>
	///// �J�����̃Z�b�g
	///// </summary>
	///// <param name="camera">�J����</param>
	//static void SetCamera(Camera* camera) {
	//	Object3Ds::camera = camera;
	//}

	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}
public:
	////�p�C�v���C������
	//PipelineSet CreatePipeline();

	//3D�I�u�W�F�N�g����
	//void CreateObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap);

	//���f���Z�b�g
	void SetModel(Model* model);

	//void SetPmdModel(PMDmodel* pmdModel);

	// ���t���[������
	bool Initialize();
	// ���t���[������
	void Update();
	// �`��
	void Draw();

public:
	ComPtr<ID3D12Resource> constBuffB0; // �萔�o�b�t�@(OBJ)
	ComPtr<ID3D12Resource> constBuffB1; // �萔�o�b�t�@(OBJ)

	//ComPtr<ID3D12Resource> PMDconstBuffB0; // �萔�o�b�t�@(PMD)

	// �F
	XMFLOAT4 color = { 1,1,1,1 };
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	// ���[�J�����W
	XMFLOAT3 position = { 0,0,0 };
	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld;

	Model* model = nullptr;

	//PMDmodel* pmdModel = nullptr;
public:
	static void MoveEyeTarget(XMFLOAT3 s) {
		eye.x += s.x;
		eye.y += s.y;
		eye.z += s.z;
	}
};