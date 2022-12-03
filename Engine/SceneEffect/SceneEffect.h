#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>

#include "Sprite/spriteCommon.h"
#include "Camera/Camera.h"
#include "pipelineSet.h"

class Wrapper;
class SceneEffect {
	static Wrapper* dx12;

private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
public: // �T�u�N���X
	// ���_�f�[�^�\����
	struct VertexPos {
		XMFLOAT3 pos;
		float scale;
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		XMMATRIX mat;	// �R�c�ϊ��s��
		XMMATRIX matBillboard;
		XMFLOAT4 color;
	};

	//
	struct sceneEffect {
		XMFLOAT3 position = {};
		float scale = 1.0f;
		XMFLOAT4 color = { 0,0,0,1 };
	};

private: // �萔
	static const int division = 50;					// ������
	static const float radius;				// ��ʂ̔��a
	static const float prizmHeight;			// ���̍���
	static const int planeCount = division * 2 + division * 2;		// �ʂ̐�
	//static const int vertexCount = planeCount * 3;		// ���_��

	//���_��
	static const int vertexCount = 1024;
	static const int indexCount = 3 * 2;

public://�ÓI�����o�֐�
	static bool staticInitalize(ID3D12Device* _dev, SIZE ret);
	//�p�C�v���C������
	static bool Createpipeline(ID3D12Device* _dev, SIZE ret);
	//�e�N�X�`���ǂݍ���
	static bool loadTexture(UINT texNumber, const wchar_t* fileName);

	static void PreDraw(ID3D12GraphicsCommandList* _cmdList);

	static void PostDraw();

	/// <summary>
	/// �r���[�s����X�V
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// �J�����Z�b�g
	/// </summary>
	/// <param name="camera"></param>
	static inline void SetCamera(Camera* camera) { SceneEffect::camera = camera; }

	static SceneEffect* Create(
		UINT texNumber,
		XMFLOAT3 position,
		XMFLOAT4 color = { 1,1,1,1 },
		XMFLOAT2 anchorpoint = { 0.0f,0.0f },
		bool isFlipX = false, bool isFlipY = false);

	SceneEffect(
		UINT texNumber,
		XMFLOAT3 position,
		XMFLOAT2 size,
		XMFLOAT4 color,
		XMFLOAT2 anchorpoint,
		bool isFlipX, bool isFlipY);

private://�����o�֐�

protected:
	//�f�o�C�X
	static ID3D12Device* device;
	//�R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;
	// �f�X�N���v�^�q�[�v
	static ComPtr<ID3D12DescriptorHeap> descHeap;

	static Camera* camera;
	static SpriteCommon spritecommon;
	static PipelineSet pipelineset;
	static XMMATRIX matBillboard;
	static XMMATRIX matBillboardY;

	// �r���[�s��
	static XMMATRIX matView;
	// �ˉe�s��
	static XMMATRIX matProjection;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// ���_�o�b�t�@
	static ComPtr<ID3D12Resource> vertBuff;
	// �e�N�X�`���o�b�t�@
	static ComPtr<ID3D12Resource> texbuff;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	static CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	static CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

	std::forward_list<sceneEffect> effects;

protected:
	//���_�΂��ӂ��A�萔�΂��ӂ�
	ComPtr<ID3D12Resource> _vertbuff = nullptr;
	ComPtr<ID3D12Resource> constBuff = nullptr;
	//���_�΂��ӂ��т�[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	float rotation = 0.0f;	//��]�p
	XMFLOAT3 position = { 0,0,0 };//���W
	XMMATRIX matWorld;	//���[���h�s��
	XMFLOAT4 color = { 1,1,1,1 };	//�F
	UINT texNumber = 0;	//�e�N�X�`���ԍ�
	XMFLOAT2 size = { 100.0f,100.0f };	//�T�C�Y
	XMFLOAT2 anchorpoint = { 0.0f,0.5f };	//�A���J�[�|�C���g
	bool isFlipX = false;//���E���]
	bool isFlipY = false;//�㉺���]
	XMFLOAT2 texLeftTop = { 0,0 };	//�e�N�X�`��������W,�n�_
	XMFLOAT2 texSize = { 100.0f,100.0f };	//�e�N�X�`���؂�o���T�C�Y
	XMFLOAT2 texBase = { 0, 0 };	// �e�N�X�`���n�_
	bool isInvisible = false;	//��\��
	float alpha = 1.0f;

public:

	bool Initalize();
	virtual void Update();
	virtual void Draw();

	/// <summary>
	/// �p�[�e�B�N������
	/// </summary>
	/// <param name="emitter">�����ꏊ</param>
	/// <param name="num">������</param>
	void CreateParticle(XMFLOAT3 emitter, XMFLOAT4 color);

public:
	void SetAlpha(float alpha) {
		this->alpha = alpha;
	}
	/// <summary>
	/// �e�N�X�`���͈͐ݒ�
	/// </summary>
	/// <param name="texBase">�e�N�X�`��������W</param>
	/// <param name="texSize">�e�N�X�`���T�C�Y</param>
	void SetTextureRect(XMFLOAT2 texBase, XMFLOAT2 texSize);

	XMFLOAT3 Pos() { return position; }
};
