#include "shaderTest.h"

ShaderTest::ShaderTest()
{
}

ShaderTest* ShaderTest::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	ShaderTest* object3d = new ShaderTest();
	if (object3d == nullptr) {
		return nullptr;
	}
	object3d->scale = { 5,5,5 };

	// ������
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}

void ShaderTest::Update()
{
	Object3Ds::Update();
}

void ShaderTest::Draw()
{
	// nullptr�`�F�b�N
	assert(device);
	assert(Object3Ds::cmdList);

	// �I�u�W�F�N�g���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
	if (model == nullptr) {
		return;
	}
	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(LoadHlsls::pipeline.at(ShaderNumber)._pipelinestate.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(LoadHlsls::pipeline.at(ShaderNumber)._rootsignature.Get());

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// ���f���`��
	model->Draw(cmdList.Get());
}
