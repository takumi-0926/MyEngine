#include "Weapon.h"

Weapon::Weapon()
{
}
Weapon::~Weapon()
{
}

Weapon* Weapon::Create(Model* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Weapon* instance = new Weapon();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->scale = XMFLOAT3(0.001f, 0.001f, 0.001f);

	// ������
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
		return nullptr;
	}

	//���f���f�[�^�Z�b�g
	if (model) {
		instance->SetModel(model);
	}

	return instance;
}

//bool Weapon::Initialize()
//{
//	Object3Ds::Initialize();
//	return true;
//}

void Weapon::Update()
{
	//matWorld = matWorld * FollowingObjectBoneMatrix;

	//Object3Ds::Update();

	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	if (!useRotMat) {
		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
		matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
		matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	}
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity();// �ό`�����Z�b�g
	matWorld *= matScale;// ���[���h�s���	�X�P�[�����O�𔽉f
	matWorld *= matRot;	 // ���[���h�s���	��]        �𔽉f
	matWorld *= matTrans;// ���[���h�s���	���s�ړ�    �𔽉f
	matWorld *= FollowingObjectBoneMatrix;

	//position.x = matWorld.r[0].m128_f32[3];
	//position.y = matWorld.r[1].m128_f32[3];
	//position.z = matWorld.r[2].m128_f32[3];

	ConstBufferDataB0* constMap = nullptr;
 	auto result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	if (FAILED(result)) { assert(0); }
	constMap->viewproj = matViewProjection;
	constMap->cameraPos = cameraPos;
	constMap->world = matWorld;
	constBuffB0->Unmap(0, nullptr);
}

void Weapon::Draw()
{
	Object3Ds::Draw();
}
