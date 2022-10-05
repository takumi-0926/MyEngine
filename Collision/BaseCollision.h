#pragma once

#include "CollisionType.h"
#include "..\object\object3D.h"

/// <summary>
/// コライダー基底クラス
/// </summary>
class BaseCollider {
public:
	BaseCollider() = default;
	//仮想デストラクタ
	virtual ~BaseCollider() = default;

	inline void SetObject(Object3Ds* _object) {
		this->object3d = _object;
	}

	inline Object3Ds* GetObject3d() {
		return object3d;
	}

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	//形状タイプ取得
	inline CollisionShapeType GEtShapeType(){
		return shapeType;
	}

protected:
	Object3Ds* object3d = nullptr;
	//形状タイプ
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
};