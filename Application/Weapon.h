#pragma once
#include "object/object3D.h"

//�莝������N���X
class Weapon : public Object3Ds {

	//�Ǐ]��{�[���s��ۑ��p
	XMMATRIX FollowingObjectBoneMatrix;

	//�Ǐ]��ԗL���t���O
	bool FollowFlag = false;

public:
	Weapon(); //�R���X�g���N�^
	~Weapon();//�f�X�g���N�^

	//�C���X�^���X����
	static Weapon* Create(Model* model);
	//������
	//bool Initialize()override;
	//�X�V
	void Update() override;
	//�`��
	void Draw() override;

public:
	//�Z�b�^�[
	inline void SetFollowingObjectBoneMatrix(XMMATRIX matrix) { 
		this->FollowingObjectBoneMatrix = matrix; 
		FollowFlag = true;
	}
};