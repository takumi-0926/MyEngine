#include "stageObject.h"

StageObject* StageObject::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	StageObject* object3d = new StageObject();
	if (object3d == nullptr) {
		return nullptr;
	}

	object3d->position.y = -40;

	// ������
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}

void StageObject::CreateHitMesh() {
	//Triangle _hit;
	//std::vector<Mesh*> mesh = model->GetMesh();
	//auto vertex = mesh.
	//_hit.p0 = Mesh::

}

StageObject::StageObject()
{
}

void StageObject::Update()
{
	Object3Ds::Update();
}

void StageObject::Draw()
{
	Object3Ds::Draw();
}
