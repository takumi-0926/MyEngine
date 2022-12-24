#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

#include <dx12Wrapper.h>
#include <forward_list>

//�r���{�[�h�I�u�W�F�N�g
class BillboardObject
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//3D�I�u�W�F�N�g�p���_�f�[�^
	struct  BillboardVertex {
		DirectX::XMFLOAT3 pos;
		float scale;
	};

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0
	{
		DirectX::XMMATRIX viewproj;
		DirectX::XMMATRIX matBillboard;
	};
	//object�P��
	struct Object {
		DirectX::XMFLOAT3 position = {};
		float scale = 10.0f;
	};

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	BillboardObject(UINT texNumber, XMFLOAT2 size);
	/// <summary>
	/// ���z�f�X�g���N�^
	/// </summary>
	virtual ~BillboardObject();
	/// �ÓI������
	/// <param name="device">�f�o�C�X</param>
	/// <returns>����</returns>
	static bool StaticInitialize(ID3D12Device * _device);

	static bool InitializeGraphicsPipeline();

	// 3D�I�u�W�F�N�g����
	static BillboardObject* Create(UINT _texNumber);

	static bool LoadTexture(UINT texNumber,const wchar_t* FlieName);

	// ���t���[������
	virtual bool Initialize();
	// ���t���[������
	virtual void Update();
	// �`��
	virtual void Draw(ID3D12GraphicsCommandList* cmdList);

	//�s��̍X�V
	void UpdateWorldMatrix();

	/// <summary>
	/// ���_�o�b�t�@�̐���
	/// </summary>
	void CreateVertexBuffers();
	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�̐���
	/// </summary>
	void CreateIndexBuffers();

private:
	static Wrapper* dx12;

	static const int vertexCount = 1;
	static const int indexCount = 3 * 2;
	// �f�o�C�X
	static Microsoft::WRL::ComPtr<ID3D12Device> device;
	// �R�}���h���X�g
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;

	//�e�N�X�`���ő喇��
	static const int objectSRVCount = 512;
	//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;
	//�e�N�X�`�����\�[�X�̔z��
	static Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[objectSRVCount];
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	static CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	static CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate;

	//���_�o�b�t�@
	static ComPtr<ID3D12Resource> vertBuff;
	// �C���f�b�N�X�o�b�t�@
	static ComPtr<ID3D12Resource> indexBuff;

	// ���_�o�b�t�@�r���[
	static D3D12_VERTEX_BUFFER_VIEW vbView;
	// �C���f�b�N�X�o�b�t�@�r���[
	static D3D12_INDEX_BUFFER_VIEW ibView;
	// ���_�f�[�^�z��
	static BillboardVertex vertices[vertexCount];
	// ���_�C���f�b�N�X�z��
	static unsigned short indices[indexCount];

	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matScale = {};
	XMMATRIX matRot = {};
	XMMATRIX matTrans = {};
	XMMATRIX matWorld = {};

	// ���_���W
	static XMFLOAT3 eye;
	// �����_���W
	static XMFLOAT3 target;
	// ������x�N�g��
	static XMFLOAT3 up;

	// �r���[�s��
	static DirectX::XMMATRIX matView;
	// �ˉe�s��
	static DirectX::XMMATRIX matProjection;

	static XMMATRIX matBillboard;

	static XMMATRIX matBillboardY;

	std::forward_list<Object> objects;

public:
	ComPtr<ID3D12Resource> constBuffB0; // �萔�o�b�t�@(OBJ)

	// ���[�J�����W
	DirectX::XMFLOAT3 position = { 0,0,0 };
	// �F
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	// ���[�J���X�P�[��
	float scale = 10.0f;
	// X,Y,Z�����̃��[�J����]�p
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	//�e�N�X�`���ԍ�
	UINT texNumber = 0;
	//�T�C�Y
	DirectX::XMFLOAT2 size = { 100.0f,100.0f };
	//�A���J�[�|�C���g
	DirectX::XMFLOAT2 anchorpoint = { 0.0f,0.5f };
	//��\��
	bool isInvisible = false;

public:
	void SetEye(XMFLOAT3 eye);
	void SetTarget(XMFLOAT3 target);
};