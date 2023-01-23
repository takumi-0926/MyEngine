#pragma once
#include "object/object3D.h"

//手持ち武器クラス
class Weapon : public Object3Ds {

	//追従先ボーン行列保存用
	XMMATRIX FollowingObjectBoneMatrix;

	//追従状態有効フラグ
	bool FollowFlag = false;

public:
	Weapon(); //コンストラクタ
	~Weapon();//デストラクタ

	//インスタンス生成
	static Weapon* Create(Model* model);
	//初期化
	//bool Initialize()override;
	//更新
	void Update() override;
	//描画
	void Draw() override;

public:
	//セッター
	inline void SetFollowingObjectBoneMatrix(XMMATRIX matrix) { 
		this->FollowingObjectBoneMatrix = matrix; 
		FollowFlag = true;
	}
};