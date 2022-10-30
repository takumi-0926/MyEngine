#pragma once
#include "PMD/pmdObject3D.h"

class Player : public PMDobject{

	/// <summary>
	/// �X�e�[�^�X
	/// </summary>
	struct Status {
		float HP;
		float Attack;
	};

public:
	/// <summary>
	/// �C���X�^���X����
	/// </summary>
	/// <param name="_model"></param>
	/// <returns></returns>
	static Player* Create(PMDmodel* _model);
	/// <summary>
	/// �X�V����
	/// </summary>
	void Update()override;
	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw()override;
};