#pragma once
#include "../sprite.h"
class PostEffect :
    public Sprite
{
    /// <summary>
    /// コンストラクタ
    /// </summary>
    PostEffect();

    /// <summary>
    /// 描画コマンド
    /// </summary>
    /// <param name="cmdList"></param>
    void Draw(ID3D12GraphicsCommandList* cmdList);
};

