#pragma once
#include <DirectXMath.h>
#include "..\includes.h"

#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

#include "PMDModel.h"
#include "dx12Wrapper.h"
#include "object/baseObject.h"
#include "Collision\CollisionInfo.h"

class Wrapper;
class PMDmodel;
class PMDobject : public BaseObject {
	friend PMDmodel;
	static Wrapper* dx12;
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

	struct Transform {
		void* operator new(size_t size);
		XMMATRIX world;
	};

	//3D�I�u�W�F�N�g�p
	static ComPtr<ID3D12RootSignature>	_rootsignature;
	static ComPtr<ID3D12PipelineState>	_pipelinestate;
	static ComPtr<ID3D12PipelineState>	_plsShadow;

	// �f�X�N���v�^�T�C�Y
	UINT descriptorHandleIncrementSize;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap = nullptr;

	//���W�ϊ�
	Transform transform;
	Transform* mappedTransform = nullptr;
	ComPtr<ID3D12Resource> transformBuff = nullptr;
	ComPtr<ID3D12Resource> transformMat = nullptr;
	ComPtr<ID3D12DescriptorHeap> transformHeap = nullptr;

private:
	//�p�C�v���C��������
	HRESULT CreateGraphicsPipelinePMD();
	//���[�g�V�O�l�`��������
	HRESULT CreateRootSignaturePMD();

public:
	PMDobject(/*Wrapper* dx12*/);
	~PMDobject();

	PMDmodel* model = nullptr;
public:
	static bool StaticInitialize(Wrapper* _dx12);
	static PMDobject* Create(PMDmodel* _model = nullptr);

	//������
	virtual bool Initialize(PMDmodel* _model);
	//�X�V
	virtual void Update();
	//�`��
	void preDraw();
	void preDrawLight();
	virtual void Draw(bool isShadow = false);

	void SetModel(PMDmodel* _model);
	//void SetScale(XMFLOAT3 _scale)  { this->scale =  _scale; }
	//void SetPosition(XMFLOAT3 _pos) { this->position = _pos; }
	//void SetMatRot(XMMATRIX rot)    { this->matRot    = rot; }

	//XMFLOAT3 GetPosition() { return position; }

	void SetCollider(BaseCollider* collider);

	virtual void OnCollision(const CollisionInfo& info) {}

private:
	ComPtr<ID3D12Resource> PMDconstBuffB1; // �萔�o�b�t�@

protected:
	BaseCollider* collider = nullptr;

	//// �F
	//XMFLOAT4 color = { 1,1,1,1 };
	//// ���[�J���X�P�[��
	//XMFLOAT3 scale = { 1,1,1 };
	//// X,Y,Z�����̃��[�J����]�p
	//XMFLOAT3 rotation = { 0,0,0 };
	//// ���[�J�����W
	//XMFLOAT3 position = { 0,0,0 };
	//// ���[�J�����[���h�ϊ��s��
	//XMMATRIX matWorld;

	//XMMATRIX matRot = XMMatrixIdentity();;
};
