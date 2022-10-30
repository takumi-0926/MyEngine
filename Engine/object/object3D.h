#pragma once
#include <DirectXMath.h>

#include "baseObject.h"
#include "Model.h"
#include "..\includes.h"
#include "..\pipelineSet.h"
#include "..\PMD\PMDmodel.h"
#include "..\Collision\CollisionInfo.h"

//3D�I�u�W�F�N�g�p���_�f�[�^
struct  Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

class Wrapper;
class BaseCollider;
class Object3Ds : public BaseObject
{
	static Wrapper* dx12;
protected: // �����o�ϐ�

	const char* name = nullptr;

	BaseCollider* collider = nullptr;
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Object3Ds() = default;
	/// <summary>
	/// ���z�f�X�g���N�^
	/// </summary>
	virtual ~Object3Ds();
	// �ÓI������
	/// <param name="device">�f�o�C�X</param>
	/// <returns>����</returns>
	static bool StaticInitialize(ID3D12Device* _device);

	// 3D�I�u�W�F�N�g����
	static Object3Ds* Create();

	//���f���Z�b�g
	void SetModel(Model* _model);
public:
	// ���t���[������
	virtual bool Initialize();
	// ���t���[������
	virtual void Update();
	// �`��
	virtual void Draw();

	//�s��̍X�V
	void UpdateWorldMatrix();

	/// <summary>
	/// ���[���h�s��擾
	/// </summary>
	/// <returns></returns>
	const XMMATRIX GetMatWorld() { return matWorld; }

	void SetCollider(BaseCollider* collider);

	virtual void OnCollision(const CollisionInfo& info){}
public:
	ComPtr<ID3D12Resource> constBuffB0; // �萔�o�b�t�@(OBJ)
	ComPtr<ID3D12Resource> constBuffB1; // �萔�o�b�t�@(OBJ)

	// �F
	XMFLOAT4 color = { 1,1,1,1 };
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	//��]�s��̎g�p��
	bool useRotMat = false;
	// ���[�J�����W
	XMFLOAT3 position = { 0,0,0 };
	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matScale, matRot, matTrans;
	XMMATRIX matWorld;

	Model* model = nullptr;

public:
	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}

	/// <summary>
	/// ���f���擾
	/// </summary>
	/// <returns></returns>
	inline Model* GetModel() { return model; }
};