#include "stage.h"

Stage* Stage::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Stage* object3d = new Stage();
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

void Stage::CreateHitMesh() {
	//Triangle _hit;
	//std::vector<Mesh*> mesh = model->GetMesh();
	//auto vertex = mesh.
	//_hit.p0 = Mesh::

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
