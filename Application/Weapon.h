#pragma once
#include "object/object3D.h"
#include "FBX/FbxLoader.h"

//手持ち武器クラス
class Weapon : public Object3Ds {

	//追従先ボーン行列保存用
	XMMATRIX FollowingObjectBoneMatrix;

	//追従状態有効フラグ
	bool FollowFlag = false;

	XMMATRIX FbxWorld;
public:
	Weapon(); //コンストラクタ
	~Weapon();//デストラクタ

	//インスタンス生成
	static Weapon* Create(Model* model);
	//初期化
	bool Initialize()override;
	//更新
	void Update() override;
	//描画
	void Draw() override;

	void OnCollision(const CollisionInfo& info)override;

public:
	//セッター
	inline void SetFollowingObjectBoneMatrix(const FbxAMatrix& matrix) {
		FollowingObjectBoneMatrix = XMMatrixIdentity();
		FbxLoader::ConvertMatrixFormFbx(&FollowingObjectBoneMatrix, matrix);
		useWorldMat = true;
		FollowFlag = true;
	}
};