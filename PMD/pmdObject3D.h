#pragma once
#include <DirectXMath.h>
#include "..\includes.h"

#include "PMDModel.h"
#include "..\Application\dx12Wrapper.h"
#include "..\object\baseObject.h"

class Wrapper;
class PMDmodel;
class PMDobject : public BaseObject {
	friend PMDmodel;
	Wrapper* dx12;
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

	// �f�X�N���v�^�T�C�Y
	UINT descriptorHandleIncrementSize;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap = nullptr;

	////���W�ϊ�
	//Transform transform;
	//Transform* mappedTransform = nullptr;
	//ComPtr<ID3D12Resource> transformBuff = nullptr;
	//ComPtr<ID3D12Resource> transformMat = nullptr;
	//ComPtr<ID3D12DescriptorHeap> transformHeap = nullptr;

	//�V�[�����\������o�b�t�@�܂��
	//ConstBufferDataB0_1* _mappedSceneData;
	//ComPtr<ID3D12Resource> _sceneConstBuff = nullptr;
	//ComPtr<ID3D12DescriptorHeap> _sceneDescHeap = nullptr;

	//�p�C�v���C��������
	HRESULT CreateGraphicsPipelinePMD();
	//���[�g�V�O�l�`��������
	HRESULT CreateRootSignaturePMD();

	//HRESULT CreateSceneView();

public:
	PMDobject(Wrapper* dx12);
	~PMDobject();

	PMDmodel model;
public:
	//�X�V
	void Update();
	//�`��
	void Draw();

	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return cmdList; }
};
