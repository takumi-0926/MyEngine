#pragma once

#include "CollisionType.h"
#include "CollisionInfo.h"
//#include"CollisionManager.h"
#include "..\object\object3D.h"

/// <summary>
/// �R���C�_�[���N���X
/// </summary>
class BaseCollider {
public:
	friend class CollisionManager;
public:
	BaseCollider() = default;
	//���z�f�X�g���N�^
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
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	//�`��^�C�v�擾
	inline CollisionShapeType GEtShapeType(){
		return shapeType;
	}

	//�����ݒ�
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
	//�`��^�C�v
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
	//�����蔻�葮��
	unsigned short attribute = 0b1111111111111111;
};