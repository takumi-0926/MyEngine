#pragma once
#include <DirectXMath.h>
#include "includes.h"

#include "dx12Wrapper.h"
#include "pipelineSet.h"
#include "PMDModel.h"

#include "baseObject.h"

class Wrapper;
class PMDmodel;
class PMDobject : public BaseObject {
	friend PMDmodel;
	friend Wrapper;
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

	//3D�I�u�W�F�N�g�p
	static ComPtr<ID3D12RootSignature>			_rootsignature;
	static ComPtr<ID3D12PipelineState>			_pipelinestate;

	//�V�[�����\������o�b�t�@�܂��
	ConstBufferDataB0_1* _mappedSceneData;
	ComPtr<ID3D12Resource> _sceneConstBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap> _sceneDescHeap = nullptr;

	//�p�C�v���C��������
	HRESULT CreateGraphicsPipelinePMD();
	//���[�g�V�O�l�`��������
	HRESULT CreateRootSignaturePMD();

	HRESULT CreateSceneView();

	HRESULT CreateDescHeap();

public:
	PMDobject();
	~PMDobject();

	//void SetPosition(XMFLOAT3 position) {
	//	this->position = position;
	//}
public:
	//�X�V
	void Update();
	//�`��
	void Draw();

	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return cmdList; }
private:
	//ComPtr<ID3D12Resource> PMDconstBuffB0;

public:
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

	//PMDmodel* model = nullptr;
};
