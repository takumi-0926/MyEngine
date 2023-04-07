#pragma once
#include "object/object3D.h"
#include "FBX/FbxLoader.h"

//�莝������N���X
class Weapon : public Object3Ds {

	//�Ǐ]��{�[���s��ۑ��p
	XMMATRIX FollowingObjectBoneMatrix;

	//�Ǐ]��ԗL���t���O
	bool FollowFlag = false;

	XMMATRIX FbxWorld;
public:
	Weapon(); //�R���X�g���N�^
	~Weapon();//�f�X�g���N�^

	//�C���X�^���X����
	static Weapon* Create(Model* model);
	//������
	bool Initialize()override;
	//�X�V
	void Update() override;
	//�`��
	void Draw() override;

	void OnCollision(const CollisionInfo& info)override;

public:
	//�Z�b�^�[
	inline void SetFollowingObjectBoneMatrix(const FbxAMatrix& matrix) {
		FollowingObjectBoneMatrix = XMMatrixIdentity();
		FbxLoader::ConvertMatrixFormFbx(&FollowingObjectBoneMatrix, matrix);
		useWorldMat = true;
		FollowFlag = true;
	}
};