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
	//�p�C�v���C�� / ���[�g�V�O�l�`��
	//static ComPtr<ID3D12PipelineState>			_pipelinestate;
	//static ComPtr<ID3D12RootSignature>			_rootsignature;

	//�V�F�[�_
	ID3DBlob* _vsBlob = nullptr; //���_�V�F�[�_�p
	ID3DBlob* _psBlob = nullptr; //�s�N�Z���V�F�[�_�p
	ID3DBlob* _gsBlob = nullptr; //�W�I���g���V�F�[�_�p

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

	// �r���[�s��
	static XMMATRIX matView;
	// �ˉe�s��
	static XMMATRIX matProjection;
	// ���_���W
	static XMFLOAT3 eye;
	// �����_���W
	static XMFLOAT3 target;
	// ������x�N�g��
	static XMFLOAT3 up;

	ComPtr<ID3D12DescriptorHeap>		_DescHeap = nullptr;	//�ėp�q�[�v

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0
	{
		//XMFLOAT4 color;	// �F (RGBA)
		//XMMATRIX mat;	// �R�c�ϊ��s��
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
		XMMATRIX pmdWorld;
	};
	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0_1
	{
		//XMFLOAT4 color;	// �F (RGBA)
		//XMMATRIX mat;	// �R�c�ϊ��s��
		XMMATRIX viewproj;
		XMFLOAT3 cameraPos;
	};


protected: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	//�p�C�v���C��
	static PipelineSet pipelineSet;

	//static Camera* camera;

public:
	// �`��O����
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList) {
		// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
		assert(BaseObject::cmdList == nullptr);

		// �R�}���h���X�g���Z�b�g
		BaseObject::cmdList = cmdList;

		// �v���~�e�B�u�`���ݒ�
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	// �`��㏈��
	static void PostDraw() {
		// �R�}���h���X�g������
		BaseObject::cmdList = nullptr;
	}

	////�J����������
	//static void InitalizeCamera(int window_width, int window_height) {
	//	// �r���[�s��̐���
	//	matView = XMMatrixLookAtLH(
	//		XMLoadFloat3(&eye),
	//		XMLoadFloat3(&target),
	//		XMLoadFloat3(&up));

	//	// ���s���e�ɂ��ˉe�s��̐���
	//	//constMap->mat = XMMatrixOrthographicOffCenterLH(
	//	//	0, window_width,
	//	//	window_height, 0,
	//	//	0, 1);
	//	// �������e�ɂ��ˉe�s��̐���
	//	matProjection = XMMatrixPerspectiveFovLH(
	//		XMConvertToRadians(60.0f),
	//		(float)window_width / window_height,
	//		0.1f, 1000.0f
	//	);
	//}

public:
	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="camera">�J����</param>
	//static void SetCamera(Camera* camera) {
	//	BaseObject::camera = camera;
	//}
};