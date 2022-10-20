#pragma once
#include "..\object\object3D.h"
#include "..\Collision\Collision.h"
#include "..\Collision\CollisionAttribute.h"

class Stage : public Object3Ds {
public:
	static Stage* Create(Model* model = nullptr);

	bool Initialize(Model* model);

	Stage();
	void Update() override;
	void Draw() override;
};