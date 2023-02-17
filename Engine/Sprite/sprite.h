#pragma once
#include "spriteCommon.h"
#include "..\pipelineSet.h"

class Sprite : public LoadHlsls {
protected://�G�C���A�X
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�X�v���C�g�p���_�f�[�^
	struct VertexPosUv {
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	/// <summary>
	/// �萔�o�b�t�@�p�f�[�^�\����
	/// </summary>
	struct ConstBufferData
	{
		XMFLOAT4 color;	// �F (RGBA)
		XMMATRIX mat;	// �R�c�ϊ��s��
	};

public://�ÓI�����o�֐�
	static bool staticInitalize(ID3D12Device* _dev, SIZE ret);
	//�p�C�v���C������
	static bool Createpipeline(ID3D12Device* _dev, SIZE ret);
	//�e�N�X�`���ǂݍ���
	static bool loadTexture(UINT texNumber, const wchar_t* fileName);

	static void PreDraw(ID3D12GraphicsCommandList* _cmdList);

	static void PostDraw();

	static Sprite* Create(
		UINT texNumber,
		XMFLOAT2 position,
		XMFLOAT4 color = { 1,1,1,1 },
		XMFLOAT2 anchorpoint = { 0.0f,0.0f },
		bool isFlipX = false, bool isFlipY = false);

	Sprite(
		UINT texNumber,
		XMFLOAT2 position,
		XMFLOAT2 size,
		XMFLOAT4 color,
		XMFLOAT2 anchorpoint,
		bool isFlipX, bool isFlipY);

private://�����o�֐�
	/// <summary>
	/// ���_�f�[�^�]��
	/// </summary>
	void TransVertex();

protected:
	//�f�o�C�X
	static ID3D12Device* device;
	//�R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;

	static SpriteCommon spritecommon;

	static PipelineSet pipelineset;

protected:
	//���_�΂��ӂ��A�萔�΂��ӂ�
	ComPtr<ID3D12Resource> _vertbuff = nullptr;
	ComPtr<ID3D12Resource> constBuff = nullptr;
	//���_�΂��ӂ��т�[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//��]�p
	float rotation = 0.0f;
	//���W
	XMFLOAT2 position = { 0,0 };
	//���[���h�s��
	XMMATRIX matWorld;

	//�F
	XMFLOAT4 color = { 1,1,1,1 };

	//�e�N�X�`���ԍ�
	UINT texNumber = 0;

	//�T�C�Y
	XMFLOAT2 size = { 100.0f,100.0f };

	//�A���J�[�|�C���g
	XMFLOAT2 anchorpoint = { 0.0f,0.5f };

	bool isFlipX = false;//���E���]
	bool isFlipY = false;//�㉺���]

	//�e�N�X�`��������W,�n�_
	XMFLOAT2 texLeftTop = { 0,0 };

	//�e�N�X�`���؂�o���T�C�Y
	XMFLOAT2 texSize = { 100.0f,100.0f };
	// �e�N�X�`���n�_
	XMFLOAT2 texBase = { 0, 0 };

	//��\��
	bool isInvisible = false;

	float alpha = 1.0f;

public:

	bool Initalize();

	virtual void Update();

	virtual void Draw();

public:
	/// <summary>
	/// �T�C�Y�ݒ�
	/// </summary>
	/// <param name="size">�T�C�Y</param>
	void SetSize(XMFLOAT2 size) {
		this->size = size;
		TransVertex();
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(XMFLOAT2 pos) {
		this->position = pos;
		TransVertex();
	}

	void MovePos(XMFLOAT2 pos) {
		this->position.x += pos.x;
		this->position.y += pos.y;
		TransVertex();
	}
	/// <summary>
	/// 
	/// </summary>
	void SetAnchorPoint(XMFLOAT2 anchorpoint) {
		this->anchorpoint = anchorpoint;
		TransVertex();
	}

	void SetAlpha(float alpha) {
		this->alpha = alpha;
	}
	/// <summary>
	/// �e�N�X�`���͈͐ݒ�
	/// </summary>
	/// <param name="texBase">�e�N�X�`��������W</param>
	/// <param name="texSize">�e�N�X�`���T�C�Y</param>
	void SetTextureRect(XMFLOAT2 texBase, XMFLOAT2 texSize);

	XMFLOAT2 Pos() { return position; }
};