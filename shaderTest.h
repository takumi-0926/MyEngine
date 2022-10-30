#pragma once
#include "object/object3D.h"

class ShaderTest : public Object3Ds{
public:
	ShaderTest();
	static ShaderTest* Create();

	void Update()override;
	void Draw();

	int ShaderNumber;
};