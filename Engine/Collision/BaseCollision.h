#pragma once

#include "CollisionType.h"
#include "CollisionInfo.h"
//#include"CollisionManager.h"
#include "object/object3D.h"
#include "FBX/FbxObject3d.h"
#include "PMD/pmdObject3D.h"

/// <summary>
/// コライダー基底クラス
/// </summary>
class BaseCollider {
public:
	friend class CollisionManager;
protected:
	Object3Ds* object3d = nullptr;

	FbxObject3d* fbxObject = nullptr;

	PMDobject* pmdObject = nullptr;

	//形状タイプ
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
	//当たり判定属性
	unsigned short attribute = 0b1111111111111111;

	unsigned short MyNumber = 0b1111111111111111;

	//当たり判定有効化
	bool invisible = true;

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

	inline void SetObject(FbxObject3d* _object) {
		this->fbxObject = _object;
	}
	inline FbxObject3d* GetFbxObject3d() {
		return fbxObject;
	}

	inline void SetObject(PMDobject* _object) {
		this->pmdObject = _object;
	}
	inline PMDobject* GetPmdObject3d() {
		return pmdObject;
	}

	inline void OnCllision(const CollisionInfo& info) {
		if (object3d != nullptr) { object3d->OnCollision(info); }
		//if (fbxObject != nullptr) { fbxObject->OnCollision(info); }
	}

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	//形状タイプ取得
	inline CollisionShapeType GetShapeType(){
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
	inline void SetMyNumber(unsigned short myNumber) {
		this->MyNumber = myNumber;
	}
	inline void SetInvisible(bool flag) {
		this->invisible = flag;
	}
};