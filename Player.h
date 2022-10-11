#pragma once
#include "..\PMD\PMDmodel.h"

class Player : public PMDmodel{

	struct Status {
		float HP;
		float Attack;
	};

	PMDmodel* player;

public:
	Player(Wrapper* _dx12, const char* filepath, PMDobject& object);
	//void Update()override;
	//void Draw(ID3D12GraphicsCommandList* cmdList)override;


};