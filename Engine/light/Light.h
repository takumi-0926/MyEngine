#pragma once
#include <DirectXMath.h>
#include <d3dx12.h>
#include <d3d12.h>

#include "DirectionalLight.h"
#include "shadow/circleShadow.h"
#include "PointLight.h"

/// <summary>
/// ���C�g
/// </summary>
class Light {
private://�G�C���A�X
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �萔
	static const int DirLightNum = 3;

	//�_�����̐�
	static const int PointLightNum = 3;

	//�Z�e�̐�
	static const int CircleShadowNum = 3;

public://�T�u�N���X
	//�萔�o�b�t�@�p�\����
	struct ConstBufferData {
		//���̐F
		XMFLOAT3 color;
		float pad;
		//���s�����p
		DirectionalLight::ConstBufferData dirRights[DirLightNum];
		// �_�����p
		PointLight::ConstBufferData pointLights[PointLightNum];
		//�Z�e�p
		CircleShadow::ConstBufferData circleShadows[CircleShadowNum];
	};

private://�ÓI�����o�ϐ�
	static ID3D12Device* device;

private://�����o�ϐ�
	ComPtr<ID3D12Resource> constBuff;

	XMFLOAT3 lightcolor = { 1,1,1 };

	DirectionalLight dirLights[DirLightNum];

	PointLight pointLights[PointLightNum];

	CircleShadow circleShadows[CircleShadowNum];

	bool dirty = false;

public://�ÓI�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device"></param>
	static void StaticInitalize(ID3D12Device* device);

	/// <summary>
	/// �C���X�^���X����
	/// </summary>
	/// <returns></returns>
	static Light* Create();

public:

	/// <summary>
	/// ����������
	/// </summary>
	void Initalize();

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList"></param>
	/// <param name="rootParameterIndex"></param>
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	/// <summary>
	/// �萔�o�b�t�@�]������
	/// </summary>
	void TarnsferConstBuffer();

	void DefaultLightSetting();

	/// ���s�����̗L���t���O�Z�b�g
	void SetDirLightActive(int index, bool active);
	/// ���s�����̃��C�g�����Z�b�g
	void SetDirLightDir(int index, const XMVECTOR& lightdir);
	/// ���s�����̃��C�g�F�Z�b�g
	void SetDirLightColor(int index, const XMFLOAT3& lightcolor);

	void SetLightColor(const XMFLOAT3& lightcolor);

	void SetPointLightActive(int index, bool active);
	void SetPointLightPos(int index, const XMFLOAT3& lightpos);
	void SetPointLightColor(int index, const XMFLOAT3& lightcolor);
	void SetPointLightAtten(int index, const XMFLOAT3& lightAtten);


	void SetCircleShadowActive(int index, bool active);
	void SetCircleShadowCasterPos(int index, const XMFLOAT3& casterPos);
	void SetCircleShadowDir(int index, const XMVECTOR& lightDir);
	void SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight);
	void SetCircleShadowAtten(int index, const XMFLOAT3& lightAtten);
	void SetCircleShadowFacterAngle(int index, const XMFLOAT2& factorAngle);
};