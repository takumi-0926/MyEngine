#pragma once

#include <DirectXMath.h>

class Object3Ds;
class FbxObject3d;
class BaseCollider;

/// <summary>
/// �Փˏ��
/// </summary>
struct CollisionInfo {
public:
	CollisionInfo(
		Object3Ds* _object,
		FbxObject3d* _fbx,
		BaseCollider* _collider, 
		const DirectX::XMVECTOR& _inter) {
		this->object = _object;
		this->collider = _collider;
		this->inter = _inter;
	}

	//�Փˑ���̃I�u�W�F�N�g
	Object3Ds* object = nullptr;

	FbxObject3d* fbx = nullptr;

	//�Փˑ���̃R���C�_�[
	BaseCollider* collider = nullptr;
	//�Փ˓_
	DirectX::XMVECTOR inter;
};