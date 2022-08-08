#pragma once
#include <DirectXMath.h>

#include "baseObject.h"
#include "Model.h"
#include "..\includes.h"
#include "..\pipelineSet.h"
#include "..\PMD\PMDmodel.h"

//3D�I�u�W�F�N�g�p���_�f�[�^
struct  Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

class Wrapper;
class Object3Ds : public BaseObject
{
	static Wrapper* dx12;
private: // �ÓI�����o�ϐ�
	//3D�I�u�W�F�N�g�p
	//static ComPtr<ID3D12RootSignature>			_rootsignature;
	//static ComPtr<ID3D12PipelineState>			_pipelinestate;

public:
	// �ÓI������
	/// <param name="device">�f�o�C�X</param>
	/// <param name="window_width">��ʕ�</param>
	/// <param name="window_height">��ʍ���</param>
	/// <returns>����</returns>
	static bool StaticInitialize(ID3D12Device* _device, SIZE _ret);

	// 3D�I�u�W�F�N�g����
	static Object3Ds* Create();

	/// <summary>
	/// �O���t�B�b�N�p�C�v���C������
	/// </summary>
	/// <returns>����</returns>
	static bool InitializeGraphicsPipeline();

public:
	//���f���Z�b�g
	void SetModel(Model* model);
	// ���t���[������
	virtual bool Initialize();
	// ���t���[������
	virtual void Update();
	// �`��
	virtual void Draw();

public:
	ComPtr<ID3D12Resource> constBuffB0; // �萔�o�b�t�@(OBJ)
	ComPtr<ID3D12Resource> constBuffB1; // �萔�o�b�t�@(OBJ)

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

public:
	static void MoveEyeTarget(XMFLOAT3 s) {
		eye.x += s.x;
		eye.y += s.y;
		eye.z += s.z;
	}
	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}
};