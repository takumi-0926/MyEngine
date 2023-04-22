//#pragma once
#include "Sprite\sprite.h"
class PostEffect :
    public Sprite
{
private:
    //�e�N�X�`���o�b�t�@
    ComPtr<ID3D12Resource> texBuff;

    //�[�x�e�N�X�`���o�b�t�@
    ComPtr<ID3D12Resource> depthBuff;
    ComPtr<ID3D12DescriptorHeap>depthHaepSRV;	//�[�x�e�N�X�`��

    //RTV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> descHeapRTV;

    //DSV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> descHeapDSV;

    //�p�C�v���C��
    ComPtr<ID3D12PipelineState> pipelineState;

    //���[�g�V�O�l�`��
    ComPtr<ID3D12RootSignature> rootSignature;

    D3D12_CPU_DESCRIPTOR_HANDLE			heapHandle_CPU;
    D3D12_GPU_DESCRIPTOR_HANDLE			heapHandle_GPU;

    //�N���A�J���[
    static const float clearColor[4];

public:
    /// <summary>
    /// �R���X�g���N�^
    /// </summary>
    PostEffect();

    /// <summary>
    /// ������
    /// </summary>
    void Initialize(ID3D12DescriptorHeap* heap);

    /// <summary>
    /// �`��R�}���h
    /// </summary>
    /// <param name="cmdList"></param>
    void Draw(ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* heap);
//
    /// <summary>
    /// �V�[���O
    /// </summary>
    /// <param name="cmdList"></param>
    void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// �V�[����
    /// </summary>
    /// <param name="cmdList"></param>
    void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// 
    /// </summary>
    void CreateGraphicsPipeline();
};

