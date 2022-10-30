#pragma once

#include "CollisionType.h"
#include "CollisionInfo.h"
//#include"CollisionManager.h"
#include "..\object\object3D.h"

/// <summary>
/// コライダー基底クラス
/// </summary>
class BaseCollider {
public:
	friend class CollisionManager;
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

	inline void OnCllision(const CollisionInfo& info) {
		object3d->OnCollision(info);
	}

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	//形状タイプ取得
	inline CollisionShapeType GEtShapeType(){
		return shapeType;
	}

	//属性設定
	inline void SetAttribute(unsigned short attribute) {
		this->attribute = attribute;
	}
	inline void AddAttribute(unsigned short attribute) {
		this->attribute |= attribute;
	}
	inline void RemoveAttribute(unsigned short attribute) {
		this-> attribute &= !attribute;
	}

protected:
	Object3Ds* object3d = nullptr;
	//形状タイプ
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
	//当たり判定属性
	unsigned short attribute = 0b1111111111111111;
};