#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>

class SpriteCommon {

public://�G�C���A�X
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�e�N�X�`���ő喇��
	static const int spriteSRVCount = 512;
	//���_��
	static const int vertNum = 4;

	//�p�C�v���C���̃Z�b�g
	//PipelineSet pipelineSet;
	//�ˉe�s��
	static XMMATRIX matProjection;
	//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
	ComPtr<ID3D12DescriptorHeap> _descHeap = nullptr;
	//�e�N�X�`�����\�[�X�̔z��
	ComPtr<ID3D12Resource> _texBuff[spriteSRVCount];
};