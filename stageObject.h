#pragma once
#include "..\object\object3D.h"
#include "..\Collision\Collision.h"

class StageObject : public Object3Ds {
	std::vector<Triangle> hit;

public:

	static StageObject* Create();

	void CreateHitMesh();

	StageObject();
	void Update() override;
	void Draw() override;
};