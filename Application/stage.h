#pragma once
#include "object\object3D.h"
#include "Collision\Collision.h"
#include "Collision\CollisionAttribute.h"

enum ObjectType {
	None = 0,
	Ground,
	Wall,
	Gate,
	FounDation,
};

class Stage : public Object3Ds {
public:
	static Stage* Create(Model* model = nullptr);

	bool Initialize(Model* model);

	Stage();
	void Update() override;
	void Draw() override;

private:
	int ObjectNum = 0;
	bool installation = false;
public:
	inline int GetObjectNum() { return this->ObjectNum; }
	inline void SetObjectNum(int num) { this->ObjectNum = num; }

	inline bool GetInstallation() { return this->installation; }
	inline void SetInstallation(bool flg) { this->installation = flg; }
};