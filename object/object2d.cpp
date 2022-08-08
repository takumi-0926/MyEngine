#include "object2d.h"
#include <wrl.h>
#include <string>

#include "..\pipelineSet.h"
#include "..\Application\application.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

ComPtr<ID3D12Device> object2d::device = nullptr;

//void object2d::CreateTryangle()
//{
//	XMFLOAT3 vertices[] = {
//		{-0.5f,-0.5f,0.0f},
//		{-0.5f,+0.5f,0.0f},
//		{+0.5f,-0.5f,0.0f}
//	};
//
//	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));
//
//	// 頂点バッファ生成
//	auto result = device->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff));
//	if (FAILED(result)) {
//		assert(0);
//		return;
//	}
//
//	// 頂点バッファへのデータ転送
//	XMFLOAT3* vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
//	for (int i = 0; i < _countof(vertices); i++) {
//		vertMap[i] = vertices[i];
//	}
//
//	vertBuff->Unmap(0, nullptr);
//
//	// 頂点バッファビューの作成
//	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	vbView.SizeInBytes = sizeVB;
//	vbView.StrideInBytes = sizeof(XMFLOAT3);
//
//	HRESULT result = S_FALSE;
//	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
//	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
//	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト
//
//	result = D3DCompileFromFile(//VS
//		L"Resources/shaders/BasicVS.hlsl",//シェーダー名
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		"BasicVS", "vs_5_0",//関数、対象シェーダー
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
//		0,
//		&vsBlob, &errorBlob);
//	if (FAILED(result)) {
//		// errorBlobからエラー内容をstring型にコピー
//		std::string errstr;
//		errstr.resize(errorBlob->GetBufferSize());
//
//		std::copy_n((char*)errorBlob->GetBufferPointer(),
//			errorBlob->GetBufferSize(),
//			errstr.begin());
//		errstr += "\n";
//		// エラー内容を出力ウィンドウに表示
//		OutputDebugStringA(errstr.c_str());
//		exit(1);
//	}
//
//	result = D3DCompileFromFile(//PS
//		L"Resources/shaders/BasicPS.hlsl",//シェーダー名
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		"BasicPS", "ps_5_0",//関数、対象シェーダー
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
//		0,
//		&psBlob, &errorBlob);
//	if (FAILED(result)) {
//		// errorBlobからエラー内容をstring型にコピー
//		std::string errstr;
//		errstr.resize(errorBlob->GetBufferSize());
//
//		std::copy_n((char*)errorBlob->GetBufferPointer(),
//			errorBlob->GetBufferSize(),
//			errstr.begin());
//		errstr += "\n";
//		// エラー内容を出力ウィンドウに表示
//		OutputDebugStringA(errstr.c_str());
//		exit(1);
//	}
//
//	// 頂点レイアウト
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
//		{ // xy座標(1行で書いたほうが見やすい)
//			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
//			D3D12_APPEND_ALIGNED_ELEMENT,
//			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
//		},
//	};
//
//	// グラフィックスパイプラインの流れを設定
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
//	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
//	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
//
//	// サンプルマスク
//	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
//	// ラスタライザステート
//	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
//	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
//	// デプスステンシルステート
//	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
//
//	// レンダーターゲットのブレンド設定
//	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
//	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
//	blenddesc.BlendEnable = true;
//	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
//	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
//	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
//
//	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
//	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
//	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
//
//	// ブレンドステートの設定
//	gpipeline.BlendState.RenderTarget[0] = blenddesc;
//
//	// 深度バッファのフォーマット
//	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
//
//	// 頂点レイアウトの設定
//	gpipeline.InputLayout.pInputElementDescs = inputLayout;
//	gpipeline.InputLayout.NumElements = _countof(inputLayout);
//
//	// 図形の形状設定（三角形）
//	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//
//	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
//	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
//	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング
//
//	// デスクリプタレンジ
//	CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
//	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ
//
//	// ルートパラメータ
//	CD3DX12_ROOT_PARAMETER rootparams[3]{};
//	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
//	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
//	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
//
//	// スタティックサンプラー
//	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
//
//	// ルートシグネチャの設定
//	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
//	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
//
//	ComPtr<ID3DBlob> rootSigBlob;
//	// バージョン自動判定のシリアライズ
//	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
//	// ルートシグネチャの生成
//	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&_rootsignature));
//	if (FAILED(result)) { return; }
//
//	gpipeline.pRootSignature = _rootsignature.Get();
//
//	// グラフィックスパイプラインの生成
//	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&_pipelinestate));
//
//}

bool object2d::StaticInitialize(ID3D12Device* _device)
{
	// 再初期化チェック
	assert(!object2d::device);

	// nullptrチェック
	assert(_device);

	object2d::device = _device;

	LoadHlsls::LoadHlsl_VS(ShaderNo::TWO, L"Resources/shaders/2dObjVS.hlsl", "main", "vs_5_0");
	LoadHlsls::LoadHlsl_PS(ShaderNo::TWO, L"Resources/shaders/2dObjPS.hlsl", "main", "ps_5_0");
	LoadHlsls::createPipeline(device.Get(), ShaderNo::TWO);

	return true;
}

object2d* object2d::Create()
{
	object2d* obj = new object2d();

	return obj;
}

//void object2d::CreateBuffer(XMFLOAT3 _vertices[], unsigned short _indices[])
//{
//	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(_vertices));
//
//	CD3DX12_HEAP_PROPERTIES heapprop(D3D12_HEAP_TYPE_UPLOAD);
//	//CD3DX12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
//	// 頂点バッファ生成
//	auto result = device->CreateCommittedResource(
//		&heapprop,
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff));
//	// 頂点バッファへのデータ転送
//	XMFLOAT3* vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
//	for (int i = 0; i < _countof(_vertices); i++) {
//		vertMap[i] = _vertices[i];
//	}
//	vertBuff->Unmap(0, nullptr);
//
//	// 頂点バッファビューの作成
//	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	vbView.SizeInBytes = sizeVB;
//	vbView.StrideInBytes = sizeof(XMFLOAT3);
//
//	if (FAILED(result)) {
//		assert(0);
//		return;
//	}
//
//	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * _countof(_indices));
//	// インデックスバッファ生成
//	result = device->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&indexBuff));
//	if (FAILED(result)) {
//		assert(0);
//		return;
//	}
//
//	// インデックスバッファへのデータ転送
//	unsigned short* indexMap = nullptr;
//	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
//	for (int i = 0; i < _countof(_indices); i++) {
//		indexMap[i] = _indices[i];
//	}
//	indexBuff->Unmap(0, nullptr);
//
//	// インデックスバッファビューの作成
//	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
//	ibView.Format = DXGI_FORMAT_R16_UINT;
//	ibView.SizeInBytes = sizeIB;
//
//}

void object2d::CreateLine(float x1, float y1, float x2, float y2)
{
	HRESULT result;

	x1 = x1 / Application::window_width;
	x2 = x2 / Application::window_width;
	y1 = y1 / Application::window_height;
	y2 = y2 / Application::window_height;

	XMFLOAT3 vertices[] = {
		{x1 - 0.001f,y1 + 0.001f,0.0f},
		{x1 + 0.001f,y1 - 0.001f,0.0f},
		{x2 + 0.001f,y2 - 0.001f,0.0f},
		{x2 - 0.001f,y2 + 0.001f,0.0f},
	};

	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	CD3DX12_HEAP_PROPERTIES heapprop(D3D12_HEAP_TYPE_UPLOAD);
	//CD3DX12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	// 頂点バッファ生成
	result = device->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	// 頂点バッファへのデータ転送
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i];
	}
	vertBuff->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	if (FAILED(result)) {
		assert(0);
		return;
	}

	unsigned short indices[] = {
		1,0,3,
		2,1,3,
	};

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * _countof(indices));
	// インデックスバッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	for (int i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	indexBuff->Unmap(0, nullptr);

	// インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
}

void object2d::CreateCircle(float x, float y, float r)
{
	HRESULT result;

	XMFLOAT3 vertices[18 + 1] = {};
	for (int i = 0; i < 18; i++) {
		vertices[i].x = r * sin((2 * 3.14) / 18 * i);
		vertices[i].y = r * cos((2 * 3.14) / 18 * i);
		vertices[i].z = 0;
	}
	vertices[18] = XMFLOAT3(0, 0, 0);

	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	CD3DX12_HEAP_PROPERTIES heapprop(D3D12_HEAP_TYPE_UPLOAD);
	//CD3DX12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	// 頂点バッファ生成
	result = device->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	// 頂点バッファへのデータ転送
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i];
	}
	vertBuff->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	if (FAILED(result)) {
		assert(0);
		return;
	}

	unsigned short indices[] = {
		1,0,3,
		2,1,3,
	};

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * _countof(indices));
	// インデックスバッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	for (int i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	indexBuff->Unmap(0, nullptr);

	// インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

}

void object2d::DarwLine(ID3D12GraphicsCommandList* cmdList)
{
	if (initFlag == false) {
		//CreateLine(float x1, float y1, float x2, float y2);
		initFlag = true;
	}

	cmdList->SetPipelineState(LoadHlsls::pipeline.at(ShaderNo::TWO)._pipelinestate.Get());
	cmdList->SetGraphicsRootSignature(LoadHlsls::pipeline.at(ShaderNo::TWO)._rootsignature.Get());

	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
