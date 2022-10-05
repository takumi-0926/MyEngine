#pragma once

#include "CollisionType.h"
#include "..\object\object3D.h"

/// <summary>
/// �R���C�_�[���N���X
/// </summary>
class BaseCollider {
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

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	//�`��^�C�v�擾
	inline CollisionShapeType GEtShapeType(){
		return shapeType;
	}

protected:
	Object3Ds* object3d = nullptr;
	//�`��^�C�v
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
};