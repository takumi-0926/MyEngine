#include "hitBox.h"
#include "..\Application\dx12Wrapper.h"

std::vector<HitBox*> HitBox::hitBox = {};
std::vector<Sqhere> HitBox::_hit = {};
Wrapper* HitBox::dx12 = nullptr;

HRESULT HitBox::CreateTransform()
{
	//auto buffSize = sizeof(XMMATRIX) * (1 + model->GetBoneMat().size());
	//buffSize = (buffSize + 0xff) & ~0xff;

	//auto result = dx12->GetDevice()->CreateCommittedResource(
	//    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
	//    D3D12_HEAP_FLAG_NONE,
	//    &CD3DX12_RESOURCE_DESC::Buffer(buffSize),
	//    D3D12_RESOURCE_STATE_GENERIC_READ,
	//    nullptr,
	//    IID_PPV_ARGS(transformBuff));
	//if (FAILED(result)) { assert(0); }

	return S_OK;
}

HitBox::HitBox()
{
}

HitBox* HitBox::Create()
{
	HitBox* hit = new HitBox();

	// ������
	if (!hit->Initialize()) {
		delete hit;
		assert(0);
		return nullptr;
	}

	return hit;
}

void HitBox::Update(std::vector<DirectX::XMMATRIX> _boneMatrices, XMFLOAT3 rot)
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rot.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rot.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rot.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	const XMMATRIX& matViewProjection = dx12->Camera()->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = dx12->Camera()->GetEye();

	// �萔�o�b�t�@�փf�[�^�]��(OBJ)
	Transform* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	if (FAILED(result)) {
		assert(0);
	}

	//constMap->mat = matWorld * matView * matProjection;	// �s��̍���
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;

	for (int i = 0; i < boneMatrices.size(); i++) {
		constMap->bones[i] = boneMatrices[i];
	}

	constBuffB0->Unmap(0, nullptr);

	this->boneMatrices = _boneMatrices;

	//Object3Ds::Update();
}

void HitBox::Draw()
{
	Object3Ds::Draw();
	//// �p�C�v���C���X�e�[�g�̐ݒ�
	//cmdList->SetPipelineState(LoadHlsls::pipeline.at(ShaderNo::HIT)._pipelinestate.Get());
	//// ���[�g�V�O�l�`���̐ݒ�
	//cmdList->SetGraphicsRootSignature(LoadHlsls::pipeline.at(ShaderNo::HIT)._rootsignature.Get());

	//// �萔�o�b�t�@�r���[���Z�b�g
	//cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	//// ���f���`��
	//model->Draw(cmdList);
}

void HitBox::mainDraw()
{
	for (int i = 0; i < hitBox.size(); i++) {
		hitBox[i]->Draw();
	}
}

void HitBox::mainUpdate(std::vector<DirectX::XMMATRIX> _boneMatrices,XMFLOAT3 rot)
{
	for (int i = 0; i < hitBox.size(); i++) {
		hitBox[i]->Update(_boneMatrices,rot);
		_hit[i].center = XMVectorSet(hitBox[i]->position.x, hitBox[i]->position.y, hitBox[i]->position.z, 1);
	}
}

void HitBox::CreateHitBox(XMFLOAT3 pos, Model* model)
{
	HitBox* hit = HitBox::Create();
	hit->SetPosition(pos);
	hit->SetModel(model);
	hitBox.push_back(hit);

	Sqhere s;
	_hit.push_back(s);
}

void HitBox::CreatePipeline(Wrapper* _dx12)
{
	CreateTransform();

	dx12 = _dx12;

	//�p�C�v���C������
	LoadHlsls::LoadHlsl_VS(ShaderNo::HIT, L"Resources/shaders/hitBoxVS.hlsl", "main", "vs_5_0");
	LoadHlsls::LoadHlsl_PS(ShaderNo::HIT, L"Resources/shaders/hitBoxPS.hlsl", "main", "ps_5_0");
	LoadHlsls::createPipeline(device, ShaderNo::HIT);
}

//void* HitBox::Transform::operator new(size_t size)
//{
//	return _aligned_malloc(size, 16);
//}
