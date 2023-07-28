#pragma once
#include <list>

#include "..\Application\enemy.h"
#include "..\Engine\FBX\FbxModel.h"

class EnemyManager {
public:

	EnemyManager();
	~EnemyManager();

	static EnemyManager* Create(FbxModel* model1,FbxModel* model2 = nullptr,FbxModel* model3 = nullptr);

	/// <summary>
	/// “G‚Ì¶¬
	/// </summary>
	void EnemyCreate();

	/// <summary>
	/// “GXVˆ—
	/// </summary>
	void Update(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos);
	/// <summary>
	/// “G•`‰æˆ—
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList,bool shadow = false);

private:

	void SetModel_1(FbxModel* model) { this->GolemModel = model; }
	void SetModel_2(FbxModel* model) { this->WolfModel = model; }

public:

	list<unique_ptr<Enemy>>& GetGolem() { return golem; }
	list<unique_ptr<Enemy>>& GetWolf() { return wolf; }

protected:

	std::list<unique_ptr<Enemy>> golem;
	std::list<unique_ptr<Enemy>> wolf;

	FbxModel* GolemModel{};
	FbxModel* WolfModel{};

	int enemyNum = {};
	float enemyPopTime = {};

	int num = 0;

public:
	int GetNum() { return num; }
};