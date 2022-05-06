#pragma once
#include <DirectXMath.h>
#include "includes.h"

#include "pipelineSet.h"
#include "PMDModel.h"

#include "baseObject.h"

class PMDModel;
class PMDobject : public BaseObject {
	friend PMDModel;
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
	static ComPtr<ID3D12PipelineState>			_pipelinestate;

	HRESULT CreateGraphicsPipeline();

	HRESULT CreateRootSignature();

public:
	PMDobject();
	~PMDobject();

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
	ComPtr<ID3D12DescriptorHeap> _sceneDescHeap = nullptr;

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
