#pragma once

#include <DirectXMath.h>

/// <summary>
/// �Z�e
/// </summary>
class CircleShadow {
private://�G�C���A�X
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		XMVECTOR				dir;//���e����
		XMFLOAT3		  casterPos;//�V���h�E�L���X�^�[
		float	distanceCasterLight;//�L���X�^�[���C�g�ԋ���
		XMFLOAT3			  atten;//���������W��
		float				   pad3;//�p�f�B���O�i�S�o�C�g�j
		XMFLOAT2	 factorAngleCos;//�p�x�����W��
		unsigned int		 active;//�A�N�e�B�u�t���O
		float				   pad4;//�p�f�B���O�i�S�o�C�g�j
	};

public://�����o�֐�
	//���e�����Z�b�g
	inline void SetDir(const XMVECTOR& dir) { this->dir = DirectX::XMVector3Normalize(dir); }
	//���e�����擾
	inline const XMVECTOR& GetDir() { return dir; }
	//�V���h�E�L���X�^�[�Z�b�g
	inline void SetCasterPos(const XMFLOAT3& casterPos) { this->casterPos = casterPos; }
	//�V���h�E�L���X�^�[�擾
	inline const XMFLOAT3& GetCasterPos() { return casterPos; }
	//�L���X�^�[���C�g�ԋ����Z�b�g
	inline void SetDistanceCasterPos(float distanceCasterLight) { this->distanceCasterLight = distanceCasterLight; }
	//�L���X�^�[���C�g�ԋ����擾
	inline float GetDistanceCasterPos() { return distanceCasterLight; }
	//���������W���Z�b�g
	inline void SetAtten(const XMFLOAT3& atten) { this->atten = atten; }
	//���������W���擾
	inline const XMFLOAT3& GetAtten() { return atten; }
	//�p�x�����W���Z�b�g
	inline void SetFactorAngle(const XMFLOAT2& factorAngle) {
		this->factorAngleCos.x = cosf(DirectX::XMConvertToRadians(factorAngle.x));
		this->factorAngleCos.y = cosf(DirectX::XMConvertToRadians(factorAngle.y));
	}
	//�p�x�����W���擾
	inline const XMFLOAT2& GetFactorAngleCos() { return factorAngleCos; }
	//�A�N�e�B�u�t���O�Z�b�g
	inline void SetActive(bool active) { this->active = active; }
	//�A�N�e�B�u�t���O�擾
	inline bool IsActive() { return active; }

private://�����o�ϐ�
	//����
	XMVECTOR dir = { 1,0,0,0 };
	//�L���X�^�[���C�g�ԋ���
	float distanceCasterLight = 1000.0f;
	//�L���X�^�[���C�g���W
	XMFLOAT3 casterPos = { 0,0,0 };
	//���������W��
	XMFLOAT3 atten = { 0.5f,0.6f,0.0f };
	//�����p�x
	XMFLOAT2 factorAngleCos = { 0.2f,0.5f };
	//�L���t���O
	bool active = false;
};