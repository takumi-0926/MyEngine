#include "object3D.h"

//void Object3Ds::InitializeObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap)
//{
//	HRESULT result;
//
//	//定数バッファのヒープ設定
//	D3D12_HEAP_PROPERTIES heapprop = {};
//	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
//
//	//定数バッファのリソース設定
//	D3D12_RESOURCE_DESC resDesc = {};
//	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	resDesc.Width = (sizeof(constBufferData) + 0xff) & ~0xff;
//	resDesc.Height = 1;
//	resDesc.DepthOrArraySize = 1;
//	resDesc.MipLevels = 1;
//	resDesc.SampleDesc.Count = 1;
//	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//	//定数バッファの生成
//	result = _dev->CreateCommittedResource(
//		&heapprop,
//		D3D12_HEAP_FLAG_NONE,
//		&resDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&object->constBuff));
//
//	UINT descHandleIncrementSize =
//		_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//
//	object->cpuDescHandleCBV = descHeap->GetCPUDescriptorHandleForHeapStart();
//	object->cpuDescHandleCBV.ptr += index * descHandleIncrementSize;
//
//	object->gpuDescHandleCBV = descHeap->GetGPUDescriptorHandleForHeapStart();
//	object->gpuDescHandleCBV.ptr += index * descHandleIncrementSize;
//
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
//	cbvDesc.BufferLocation = object->constBuff->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = (UINT)object->constBuff->GetDesc().Width;
//	_dev->CreateConstantBufferView(&cbvDesc, object->cpuDescHandleCBV);
//}
//
//void Object3Ds::UpdateObject3D(Object3D* object, XMMATRIX& viewMat, XMMATRIX& projMat)
//{
//	XMMATRIX scalMat, rotMat, transMat;
//
//	//スケール、回転、平行移動行列の計算
//	scalMat = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
//	rotMat = XMMatrixIdentity();
//	rotMat *= XMMatrixRotationZ(XMConvertToRadians(object->rotation.z));
//	rotMat *= XMMatrixRotationX(XMConvertToRadians(object->rotation.x));
//	rotMat *= XMMatrixRotationY(XMConvertToRadians(object->rotation.y));
//	transMat = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);
//
//	//ワールド行列の計算
//	object->worldMat = XMMatrixIdentity();
//	object->worldMat *= scalMat;
//	object->worldMat *= rotMat;
//	object->worldMat *= transMat;
//
//	//親オブジェクトがあれば
//	if (object->parent != nullptr) {
//		//親オブジェクトのワールド行列を掛ける
//		object->worldMat *= object->parent->worldMat;
//	}
//
//	//定数バッファにデータを転送
//	constBufferData* constMap = nullptr;
//	if (SUCCEEDED(object->constBuff->Map(0, nullptr, (void**)&constMap))) {
//		constMap->color = XMFLOAT4(1, 1, 1, 1);
//		constMap->mat = object->worldMat * viewMat * projMat;
//		object->constBuff->Unmap(0, nullptr);
//	}
//
//}
//
//void Object3Ds::DrawObject3D(Object3D* object, ID3D12GraphicsCommandList* _cmdList, ID3D12DescriptorHeap* descHeap, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV, UINT numIndices) {
//	_cmdList->IASetVertexBuffers(0, 1, &vbView);
//	_cmdList->IASetIndexBuffer(&ibView);
//	ID3D12DescriptorHeap* ppHeaps[] = { descHeap };
//	_cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
//	//定数バッファビューをセット
//	_cmdList->SetGraphicsRootConstantBufferView(0, object->constBuff->GetGPUVirtualAddress());
//	//シェーダーリソースビューをセット
//	_cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);
//	//描画コマンド
//	_cmdList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
//}
//
//PipelineSet Object3Ds::CreatePipeline(ID3D12Device* _dev)
//{
//	LoadHlsl(L"BasicVS.hlsl", &_vsBlob, "main", "vs_5_0");
//	LoadHlsl(L"BasicPS.hlsl", &_psBlob, "main", "ps_5_0");
//
//	// 頂点レイアウト
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
//		//頂点座標
//		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//法線
//		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//uv
//		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//ボーン番号
//		{"BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//ウェイト
//		{"WEIGHT",0,DXGI_FORMAT_R8_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//輪郭線フラグ
//		{"EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//	};
//
//	gpipeline.pRootSignature = nullptr;//後で設定
//
//	//シェーダー
//	gpipeline.VS = CD3DX12_SHADER_BYTECODE(_vsBlob);
//	//gpipeline.GS = CD3DX12_SHADER_BYTECODE(_gsBlob.Get());
//	gpipeline.PS = CD3DX12_SHADER_BYTECODE(_psBlob);
//
//	//サンプルマスク,ラスタライザー
//	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//背面カリングあり
//	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//背面カリングなし
//
//	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//デフォルトのサンプルマスクを表す定数
//
//	//レンダターゲットの設定
//	gpipeline.NumRenderTargets = 1;//今は1つ
//	gpipeline.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;//0～1に正規化されたRGBA
//
//	//深度ステンシル
//	gpipeline.DepthStencilState.DepthEnable = true;//使う
//	gpipeline.DepthStencilState.StencilEnable = false;//
//	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込む
//	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さい方を採用
//	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
//
//	//ブレンドステートの設定
//	gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//
//	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlenddesc = {};
//	renderTargetBlenddesc.BlendEnable = false;
//	renderTargetBlenddesc.LogicOpEnable = false;
//	renderTargetBlenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//	gpipeline.BlendState.RenderTarget[0] = renderTargetBlenddesc;
//
//	//入力レイアウトの設定
//	gpipeline.InputLayout.pInputElementDescs = inputLayout;//レイアウトの先頭アドレス
//	gpipeline.InputLayout.NumElements = _countof(inputLayout);//レイアウト配列の要素
//	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//カットなし
//	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形で構成
//
//	//アンチエイリアシングのためのサンプル数設定
//	gpipeline.SampleDesc.Count = 1;
//	gpipeline.SampleDesc.Quality = 0;
//
//	PipelineSet pipelineSet;
//	//デスクリプタレンジ
//	//定数用
//	CD3DX12_DESCRIPTOR_RANGE descRangeCBV = {};
//	descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
//
//	//テクスチャ用
//	CD3DX12_DESCRIPTOR_RANGE descRangeSRV = {};
//	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
//
//	//サンプラー
//	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};
//	samplerDesc.Init(0);
//
//	//ルートパラメーター
//	CD3DX12_ROOT_PARAMETER rootparam[2] = {};
//	rootparam[0].InitAsConstantBufferView(0);
//	rootparam[1].InitAsDescriptorTable(1, &descRangeSRV);
//
//	//ルートシグネチャ
//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
//	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//	rootSignatureDesc.pParameters = rootparam;
//	rootSignatureDesc.NumParameters = 2;
//	rootSignatureDesc.pStaticSamplers = &samplerDesc;
//	rootSignatureDesc.NumStaticSamplers = 1;
//
//	//バージョン自動生成でのシリアライズ
//	auto result = D3D12SerializeRootSignature(
//		&rootSignatureDesc,
//		D3D_ROOT_SIGNATURE_VERSION_1_0,
//		&_rootSigBlob,
//		_errorBlob.ReleaseAndGetAddressOf());
//
//	//ルートシグネチャの生成
//	result = _dev->CreateRootSignature(
//		0,
//		_rootSigBlob->GetBufferPointer(),
//		_rootSigBlob->GetBufferSize(),
//		IID_PPV_ARGS(pipelineSet._rootsignature.ReleaseAndGetAddressOf()));
//
//	//パイプラインにルートシグネチャをセット
//	gpipeline.pRootSignature = pipelineSet._rootsignature.Get();
//
//	//グラフィックスパイプラインステートオブジェクトの生成
//	result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelineSet._pipelinestate.ReleaseAndGetAddressOf()));
//
//	return pipelineSet;
//}
//
//void Object3Ds::CreateObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap)
//{
//	for (int i = 0; i < index; i++) {
//		//大きさ、回転角、座標を指定
//		//object3ds[i].parent = &object3ds[i - 1];
//		object[i].scale = { 1.0f,1.0f,1.0f };
//		object[i].rotation = { 0.0f,0.0f,0.0f };
//		object[i].position = { 0.0f,5.0f,100.0f };
//		object[i].Flag = false;
//	}
//}
