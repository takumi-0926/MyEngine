#include "baseObject.h"

// デバイス
ComPtr<ID3D12Device> BaseObject::device = nullptr;
// コマンドリスト
ComPtr<ID3D12GraphicsCommandList> BaseObject::cmdList = nullptr;

void BaseObject::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(BaseObject::cmdList == nullptr);

	// コマンドリストをセット
	BaseObject::cmdList = cmdList;

	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void BaseObject::PostDraw()
{
	// コマンドリストを解除
	BaseObject::cmdList = nullptr;
}
