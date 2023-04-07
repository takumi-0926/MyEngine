#include "Weapon.h"

#include "Collision/MeshCollider.h"
#include <Collision/CollisionAttribute.h>

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

	//instance->scale = XMFLOAT3(0.1f, 0.1f, 0.1f);

	//���f���f�[�^�Z�b�g
	if (model) {
		instance->SetModel(model);
	}

	// ������
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
		return nullptr;
	}


	return instance;
}

bool Weapon::Initialize()
{
	Object3Ds::Initialize();

	//�R���C�_�[�ǉ�
	MeshCollider* collider = new MeshCollider();
	collider->ConstructTriangles(model);
	SetCollider(collider);
	collider->SetAttribute(COLLISION_ATTR_ALLIES);
	collider->SetInvisible(true);
	return true;
}

void Weapon::Update()
{
	Object3Ds::Update();

	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity();// �ό`�����Z�b�g
	matWorld *= matScale;// ���[���h�s���	�X�P�[�����O�𔽉f
	matWorld *= matRot;	 // ���[���h�s���	��]        �𔽉f
	matWorld *= matTrans;// ���[���h�s���	���s�ړ�    �𔽉f
	matWorld *= FollowingObjectBoneMatrix;

	ConstBufferDataB0* constMap = nullptr;
	auto result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	if (FAILED(result)) { assert(0); }
	constMap->viewproj = matViewProjection;
	constMap->cameraPos = cameraPos;
	constMap->world = matWorld;
	constBuffB0->Unmap(0, nullptr);

	collider->Update();
}

void Weapon::Draw()
{
	Object3Ds::Draw();
}

void Weapon::OnCollision(const CollisionInfo& info)
{
}
