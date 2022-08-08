#pragma once
#include "..\object\object3D.h"
#include "..\Collision\Collision.h"

class Stage : public Object3Ds {
	std::vector<Triangle> hit;

public:

	static Stage* Create();

	void CreateHitMesh();

	Stage();
	void Update() override;
	void Draw() override;
};