#pragma once
#include "PMD/pmdObject3D.h"

class Player : public PMDobject{

	/// <summary>
	/// ステータス
	/// </summary>
	struct Status {
		float HP;
		float Attack;
	};

public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <param name="_model"></param>
	/// <returns></returns>
	static Player* Create(PMDmodel* _model);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override;
};