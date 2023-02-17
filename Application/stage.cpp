#include "stage.h"
#include "Collision/MeshCollider.h"

Stage* Stage::Create(Model* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Stage* instance = new Stage();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	if (!instance->Initialize(model)) {
		delete instance;
		assert(0);
		return nullptr;
	}

	return instance;
}

bool Stage::Initialize(Model* model)
{
	if (!Object3Ds::Initialize()) {
		return false;
	}

	SetModel(model);

	//�R���C�_�[�ǉ�
	MeshCollider* collider = new MeshCollider();
	collider->ConstructTriangles(model);
	SetCollider(collider);

	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	Update();
	return true;
}

Stage::Stage()
{
}

void Stage::Update()
{
	Object3Ds::Update();
}

void Stage::Draw()
{
	Object3Ds::Draw();
}