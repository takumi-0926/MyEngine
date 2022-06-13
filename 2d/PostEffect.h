#pragma once
#include "../sprite.h"
class PostEffect :
    public Sprite
{
private:
    //�e�N�X�`���o�b�t�@
    ComPtr<ID3D12Resource> texBuff;

    //SRV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> descHeapSRV;

public:
    /// <summary>
    /// �R���X�g���N�^
    /// </summary>
    PostEffect();

    /// <summary>
    /// ������
    /// </summary>
    void Initialize();

    /// <summary>
    /// �`��R�}���h
    /// </summary>
    /// <param name="cmdList"></param>
    void Draw(ID3D12GraphicsCommandList* cmdList);
};

