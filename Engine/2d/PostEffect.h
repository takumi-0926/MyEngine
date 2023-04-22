//#pragma once
#include "Sprite\sprite.h"
class PostEffect :
    public Sprite
{
private:
    //テクスチャバッファ
    ComPtr<ID3D12Resource> texBuff;

    //深度テクスチャバッファ
    ComPtr<ID3D12Resource> depthBuff;
    ComPtr<ID3D12DescriptorHeap>depthHaepSRV;	//深度テクスチャ

    //RTV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapRTV;

    //DSV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapDSV;

    //パイプライン
    ComPtr<ID3D12PipelineState> pipelineState;

    //ルートシグネチャ
    ComPtr<ID3D12RootSignature> rootSignature;

    D3D12_CPU_DESCRIPTOR_HANDLE			heapHandle_CPU;
    D3D12_GPU_DESCRIPTOR_HANDLE			heapHandle_GPU;

    //クリアカラー
    static const float clearColor[4];

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    PostEffect();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(ID3D12DescriptorHeap* heap);

    /// <summary>
    /// 描画コマンド
    /// </summary>
    /// <param name="cmdList"></param>
    void Draw(ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* heap);
//
    /// <summary>
    /// シーン前
    /// </summary>
    /// <param name="cmdList"></param>
    void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// シーン後
    /// </summary>
    /// <param name="cmdList"></param>
    void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// 
    /// </summary>
    void CreateGraphicsPipeline();
};

