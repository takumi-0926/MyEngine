#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>

class SpriteCommon {

public://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//テクスチャ最大枚数
	static const int spriteSRVCount = 512;
	//頂点数
	static const int vertNum = 4;

	//パイプラインのセット
	//PipelineSet pipelineSet;
	//射影行列
	static XMMATRIX matProjection;
	//テクスチャ用デスクリプタヒープの生成
	ComPtr<ID3D12DescriptorHeap> _descHeap = nullptr;
	//テクスチャリソースの配列
	ComPtr<ID3D12Resource> _texBuff[spriteSRVCount];
};