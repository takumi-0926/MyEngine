#pragma once
#include <DirectXMath.h>
#include "..\includes.h"

#include "..\pipelineSet.h"
#include "..\Camera\Camera.h"

//�V�F�[�_�ɓn�����߂̍s��f�[�^
struct constBufferData {
	XMFLOAT4 color;//�F(RGBA)
	XMMATRIX mat;
};

//�o�b�t�@�f�[�^��
const int constantBufferNum = 128;

class BaseObject{
protected:

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0
	{
		//XMFLOAT4 color;	// �F (RGBA)
		//XMMATRIX mat;	// �R�c�ϊ��s��
		//XMMATRIX view;
		//XMMATRIX proj;
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
		XMMATRIX lightCamera;
	};
	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0_1
	{
		//XMFLOAT4 color;	// �F (RGBA)
		//XMMATRIX mat;	// �R�c�ϊ��s��
		//XMMATRIX view;
		//XMMATRIX proj;
		XMMATRIX viewproj;
		XMFLOAT3 cameraPos;
	};

protected: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ComPtr<ID3D12Device> device;
	// �R�}���h���X�g
	static ComPtr<ID3D12GraphicsCommandList> cmdList;

	// �r���[�s��
	static XMMATRIX matView;
	// �ˉe�s��
	static XMMATRIX matProjection;

public:
	// �`��O����
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	// �`��㏈��
	static void PostDraw();
};