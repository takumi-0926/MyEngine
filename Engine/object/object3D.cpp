#include "object3D.h"
#include "dx12Wrapper.h"
#include "pipelineSet.h"
#include "Collision\BaseCollision.h"
#include "Collision\CollisionManager.h"

Wrapper* Object3Ds::dx12 = nullptr;
Camera* Object3Ds::camera = nullptr;

Object3Ds::~Object3Ds()
{
	if (collider) {
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}
}

bool Object3Ds::StaticInitialize(ID3D12Device* _device)
{
	// �ď������`�F�b�N
	assert(!Object3Ds::device);

	// nullptr�`�F�b�N
	assert(_device);

	Object3Ds::device = _device;

	//�p�C�v���C������
	LoadHlsls::LoadHlsl_VS(ShaderNo::OBJ, L"Resources/shaders/OBJVertexShader.hlsl", "main", "vs_5_0");
	LoadHlsls::LoadHlsl_PS(ShaderNo::OBJ, L"Resources/shaders/OBJPixelShader.hlsl", "main", "ps_5_0");
	LoadHlsls::createPipeline(device.Get(), ShaderNo::OBJ);

	Model::StaticInitialize(_device);

	return true;
}

Object3Ds* Object3Ds::Create(Model* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Object3Ds* object3d = new Object3Ds();
	if (object3d == nullptr) {
		return nullptr;
	}

	// ������
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	if (model) {
		object3d->SetModel(model);
	}

	return object3d;
}

void Object3Ds::SetModel(Model* model)
{
	this->model = model;
}

bool Object3Ds::Initialize()
{
	// nullptr�`�F�b�N
	assert(device);

	this->dx12 = dx12;

	name = typeid(*this).name();

	HRESULT result;
	// �萔�o�b�t�@�̐���
	CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);
	result = device->CreateCommittedResource(
		&properties, 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	return true;
}

void Object3Ds::Update()
{
	HRESULT result;
	//const XMMATRIX& matView = dx12->Camera()->GetViewMatrix();
	//const XMMATRIX& matProjection = dx12->Camera()->GetProjectionMatrix();

	if (!useWorldMat) {
		const XMMATRIX& matViewProjection = dx12->Camera()->GetViewProjectionMatrix();
		const XMFLOAT3& cameraPos = dx12->Camera()->GetEye();

		UpdateWorldMatrix();

		// �萔�o�b�t�@�փf�[�^�]��(OBJ)
		ConstBufferDataB0* constMap = nullptr;
		result = constBuffB0->Map(0, nullptr, (void**)&constMap);
		if (FAILED(result)) { assert(0); }
		constMap->viewproj = matViewProjection;
		constMap->cameraPos = cameraPos;
		if (!useWorldMat) {
			constMap->world = matWorld;
		}
		constBuffB0->Unmap(0, nullptr);
	}

	if (collider) {
		collider->Update();
	}
}

void Object3Ds::Draw()
{
	// nullptr�`�F�b�N
	assert(device);
	assert(Object3Ds::cmdList);

	// �I�u�W�F�N�g���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
	if (model == nullptr) {
		return;
	}

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(LoadHlsls::pipeline.at(ShaderNo::OBJ)._pipelinestate.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(LoadHlsls::pipeline.at(ShaderNo::OBJ)._rootsignature.Get());

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	dx12->DrawLight(cmdList.Get());

	// ���f���`��
	model->Draw(cmdList.Get());
}

void Object3Ds::UpdateWorldMatrix()
{
	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	if (!useRotMat) {
		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
		matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
		matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	}
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity();// �ό`�����Z�b�g
	matWorld *= matScale;// ���[���h�s���	�X�P�[�����O�𔽉f
	matWorld *= matRot;	 // ���[���h�s���	��]        �𔽉f
	matWorld *= matTrans;// ���[���h�s���	���s�ړ�    �𔽉f
}

void Object3Ds::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	CollisionManager::GetInstance()->AddCollider(collider);
	UpdateWorldMatrix();
	collider->Update();
}
