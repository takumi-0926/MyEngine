#pragma once
#include "..\PMD\PMDmodel.h"
#include "..\object\object3D.h"

class Object3D;
class Player : public PMDmodel{

	struct Status {
		float HP;
		float Attack;
	};

	shared_ptr<PMDmodel> player;
	Object3Ds* HitBox[];

public:
	Player(Wrapper* _dx12, const char* filepath, PMDobject& object);
	//void Initialize() override;
	void Update()override;
	void Draw(ID3D12GraphicsCommandList* cmdList)override;


};