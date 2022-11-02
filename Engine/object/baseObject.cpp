#include "baseObject.h"

// �f�o�C�X
ComPtr<ID3D12Device> BaseObject::device = nullptr;
// �R�}���h���X�g
ComPtr<ID3D12GraphicsCommandList> BaseObject::cmdList = nullptr;

void BaseObject::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
	assert(BaseObject::cmdList == nullptr);

	// �R�}���h���X�g���Z�b�g
	BaseObject::cmdList = cmdList;

	// �v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void BaseObject::PostDraw()
{
	// �R�}���h���X�g������
	BaseObject::cmdList = nullptr;
}
