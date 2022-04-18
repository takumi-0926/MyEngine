#pragma once
#include <DirectXMath.h>
#include "includes.h"

#include "pipelineSet.h"
#include "PMDModel.h"

#include "baseObject.h"

class PMDobject : public BaseObject {
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	//std::�ȗ�
	using string = std::string;

private:
	//3D�I�u�W�F�N�g�p
	static ComPtr<ID3D12RootSignature>			_rootsignature;
	//static ComPtr<ID3DBlob>						_rootSigBlob;
	static ComPtr<ID3D12PipelineState>			_pipelinestate;

private:
	//�w�b�_�[
	struct PMDHeader {
		float vertion;
		char model_name[20];
		char comment[256];
	};

	//�}�e���A���\���́i�r���Ŏg��Ȃ��Ȃ�j
	struct PMDMaterial {
		XMFLOAT3 diffuse;//�f�B�t���[�Y�F
		float alpha;	//�f�B�t���[�Y��
		float specularStrength;//�X�y�L�����̋����i��Z�l�j
		XMFLOAT3 specular;//�X�y�L�����F
		XMFLOAT3 ambient;//�A���r�G���g�F
		unsigned char toonIdx;//�g�D�[���ԍ�
		unsigned char edgeFlg;//�}�e���A�����Ƃ̗֊s���t���O

		//�Q�o�C�g�̃p�f�B���O����

		unsigned int indicesNum;//���̃}�e���A�������蓖�Ă���

		char texFilePath[20];//�e�N�X�`���t�@�C���p�X+��

	};//�v�V�O�o�C�g�����A�Q�o�C�g�̃p�f�B���O�����邽�߂V�Q�o�C�g�ɂȂ�

	//�V�F�[�_�ɓ�������}�e���A���f�[�^
	struct MaterialForHlsl {
		XMFLOAT3 diffuse;//�f�B�t���[�Y�F
		float alpha;	//�f�B�t���[�Y��
		XMFLOAT3 specular;//�X�y�L�����F
		float specularStrength;//�X�y�L�����̋����i��Z�l�j
		XMFLOAT3 ambient;//�A���r�G���g�F
	};

	//��L�ȊO�̃}�e���A���f�[�^
	struct AdditionalMaterial {
		string texpath;
		int toonIdx;
		bool edgeFlg;
	};

	//�S�̂��܂Ƃ߂�f�[�^
	struct Material {
		unsigned int indicesNum;
		MaterialForHlsl material;
		AdditionalMaterial additional;
	};

public:
	// �ÓI������
	/// <param name="device">�f�o�C�X</param>
	/// <param name="window_width">��ʕ�</param>
	/// <param name="window_height">��ʍ���</param>
	/// <returns>����</returns>
	static bool StaticInitialize(ID3D12Device* device, SIZE ret);

	/// <summary>
	/// �O���t�B�b�N�p�C�v���C������
	/// </summary>
	/// <returns>����</returns>
	static bool InitializeGraphicsPipeline();

	/// <summary>
	/// ���f������
	/// </summary>
	/// <returns></returns>
	static PMDobject* Create();

	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}
public:
	//���f���Z�b�g
	void SetModel(PMDmodel* model);
	//������
	bool Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw();

private:
	ComPtr<ID3D12Resource> PMDconstBuffB0;

public:
	// �F
	XMFLOAT4 color = { 1,1,1,1 };
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	// ���[�J�����W
	XMFLOAT3 position = { 0,0,0 };
	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld;

	PMDmodel* model = nullptr;
};
