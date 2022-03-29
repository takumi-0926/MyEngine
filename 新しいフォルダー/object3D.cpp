#include "object3D.h"

//void Object3Ds::InitializeObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap)
//{
//	HRESULT result;
//
//	//�萔�o�b�t�@�̃q�[�v�ݒ�
//	D3D12_HEAP_PROPERTIES heapprop = {};
//	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
//
//	//�萔�o�b�t�@�̃��\�[�X�ݒ�
//	D3D12_RESOURCE_DESC resDesc = {};
//	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	resDesc.Width = (sizeof(constBufferData) + 0xff) & ~0xff;
//	resDesc.Height = 1;
//	resDesc.DepthOrArraySize = 1;
//	resDesc.MipLevels = 1;
//	resDesc.SampleDesc.Count = 1;
//	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//	//�萔�o�b�t�@�̐���
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
//	//�X�P�[���A��]�A���s�ړ��s��̌v�Z
//	scalMat = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
//	rotMat = XMMatrixIdentity();
//	rotMat *= XMMatrixRotationZ(XMConvertToRadians(object->rotation.z));
//	rotMat *= XMMatrixRotationX(XMConvertToRadians(object->rotation.x));
//	rotMat *= XMMatrixRotationY(XMConvertToRadians(object->rotation.y));
//	transMat = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);
//
//	//���[���h�s��̌v�Z
//	object->worldMat = XMMatrixIdentity();
//	object->worldMat *= scalMat;
//	object->worldMat *= rotMat;
//	object->worldMat *= transMat;
//
//	//�e�I�u�W�F�N�g�������
//	if (object->parent != nullptr) {
//		//�e�I�u�W�F�N�g�̃��[���h�s����|����
//		object->worldMat *= object->parent->worldMat;
//	}
//
//	//�萔�o�b�t�@�Ƀf�[�^��]��
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
//	//�萔�o�b�t�@�r���[���Z�b�g
//	_cmdList->SetGraphicsRootConstantBufferView(0, object->constBuff->GetGPUVirtualAddress());
//	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
//	_cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);
//	//�`��R�}���h
//	_cmdList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
//}
//
//PipelineSet Object3Ds::CreatePipeline(ID3D12Device* _dev)
//{
//	LoadHlsl(L"BasicVS.hlsl", &_vsBlob, "main", "vs_5_0");
//	LoadHlsl(L"BasicPS.hlsl", &_psBlob, "main", "ps_5_0");
//
//	// ���_���C�A�E�g
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
//		//���_���W
//		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//�@��
//		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//uv
//		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//�{�[���ԍ�
//		{"BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//�E�F�C�g
//		{"WEIGHT",0,DXGI_FORMAT_R8_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//�֊s���t���O
//		{"EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//	};
//
//	gpipeline.pRootSignature = nullptr;//��Őݒ�
//
//	//�V�F�[�_�[
//	gpipeline.VS = CD3DX12_SHADER_BYTECODE(_vsBlob);
//	//gpipeline.GS = CD3DX12_SHADER_BYTECODE(_gsBlob.Get());
//	gpipeline.PS = CD3DX12_SHADER_BYTECODE(_psBlob);
//
//	//�T���v���}�X�N,���X�^���C�U�[
//	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//�w�ʃJ�����O����
//	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�w�ʃJ�����O�Ȃ�
//
//	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�f�t�H���g�̃T���v���}�X�N��\���萔
//
//	//�����_�^�[�Q�b�g�̐ݒ�
//	gpipeline.NumRenderTargets = 1;//����1��
//	gpipeline.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;//0�`1�ɐ��K�����ꂽRGBA
//
//	//�[�x�X�e���V��
//	gpipeline.DepthStencilState.DepthEnable = true;//�g��
//	gpipeline.DepthStencilState.StencilEnable = false;//
//	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//��������
//	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//�����������̗p
//	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
//
//	//�u�����h�X�e�[�g�̐ݒ�
//	gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//
//	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlenddesc = {};
//	renderTargetBlenddesc.BlendEnable = false;
//	renderTargetBlenddesc.LogicOpEnable = false;
//	renderTargetBlenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//	gpipeline.BlendState.RenderTarget[0] = renderTargetBlenddesc;
//
//	//���̓��C�A�E�g�̐ݒ�
//	gpipeline.InputLayout.pInputElementDescs = inputLayout;//���C�A�E�g�̐擪�A�h���X
//	gpipeline.InputLayout.NumElements = _countof(inputLayout);//���C�A�E�g�z��̗v�f
//	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//�J�b�g�Ȃ�
//	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//�O�p�`�ō\��
//
//	//�A���`�G�C���A�V���O�̂��߂̃T���v�����ݒ�
//	gpipeline.SampleDesc.Count = 1;
//	gpipeline.SampleDesc.Quality = 0;
//
//	PipelineSet pipelineSet;
//	//�f�X�N���v�^�����W
//	//�萔�p
//	CD3DX12_DESCRIPTOR_RANGE descRangeCBV = {};
//	descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
//
//	//�e�N�X�`���p
//	CD3DX12_DESCRIPTOR_RANGE descRangeSRV = {};
//	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
//
//	//�T���v���[
//	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};
//	samplerDesc.Init(0);
//
//	//���[�g�p�����[�^�[
//	CD3DX12_ROOT_PARAMETER rootparam[2] = {};
//	rootparam[0].InitAsConstantBufferView(0);
//	rootparam[1].InitAsDescriptorTable(1, &descRangeSRV);
//
//	//���[�g�V�O�l�`��
//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
//	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//	rootSignatureDesc.pParameters = rootparam;
//	rootSignatureDesc.NumParameters = 2;
//	rootSignatureDesc.pStaticSamplers = &samplerDesc;
//	rootSignatureDesc.NumStaticSamplers = 1;
//
//	//�o�[�W�������������ł̃V���A���C�Y
//	auto result = D3D12SerializeRootSignature(
//		&rootSignatureDesc,
//		D3D_ROOT_SIGNATURE_VERSION_1_0,
//		&_rootSigBlob,
//		_errorBlob.ReleaseAndGetAddressOf());
//
//	//���[�g�V�O�l�`���̐���
//	result = _dev->CreateRootSignature(
//		0,
//		_rootSigBlob->GetBufferPointer(),
//		_rootSigBlob->GetBufferSize(),
//		IID_PPV_ARGS(pipelineSet._rootsignature.ReleaseAndGetAddressOf()));
//
//	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
//	gpipeline.pRootSignature = pipelineSet._rootsignature.Get();
//
//	//�O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐���
//	result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelineSet._pipelinestate.ReleaseAndGetAddressOf()));
//
//	return pipelineSet;
//}
//
//void Object3Ds::CreateObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap)
//{
//	for (int i = 0; i < index; i++) {
//		//�傫���A��]�p�A���W���w��
//		//object3ds[i].parent = &object3ds[i - 1];
//		object[i].scale = { 1.0f,1.0f,1.0f };
//		object[i].rotation = { 0.0f,0.0f,0.0f };
//		object[i].position = { 0.0f,5.0f,100.0f };
//		object[i].Flag = false;
//	}
//}
