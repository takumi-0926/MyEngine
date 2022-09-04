#include "pmdObject3D.h"
#include "..\pipelineSet.h"

ComPtr<ID3D12RootSignature> PMDobject::_rootsignature;
ComPtr<ID3D12PipelineState> PMDobject::_pipelinestate;

void PMDobject::Update()
{
	//HRESULT result;
	//_mappedSceneData = nullptr;//�}�b�v��������|�C���^
	//result = _sceneConstBuff->Map(0, nullptr, (void**)&_mappedSceneData);//�}�b�v

	//const XMMATRIX& matViewProjection = dx12->Camera()->GetViewProjectionMatrix();
	//const XMFLOAT3& cameraPos = dx12->Camera()->GetEye();

	//_mappedSceneData->viewproj = matViewProjection;
	//_mappedSceneData->cameraPos = cameraPos;

	//_sceneConstBuff->Unmap(0, nullptr);
}

void PMDobject::Draw()
{
	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(_pipelinestate.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(_rootsignature.Get());
}

PMDobject::PMDobject(Wrapper* dx12):
model(model)
{
	this->dx12 = dx12;
	assert(SUCCEEDED(CreateRootSignaturePMD()));
	assert(SUCCEEDED(CreateGraphicsPipelinePMD()));
}

PMDobject::~PMDobject()
{
}

HRESULT PMDobject::CreateGraphicsPipelinePMD()
{
	////�p�C�v���C������
	//LoadHlsls::LoadHlsl_VS(ShaderNo::PMD, L"Resources/shaders/BasicVertexShader.hlsl", "BasicVS", "vs_5_0");
	//LoadHlsls::LoadHlsl_PS(ShaderNo::PMD, L"Resources/shaders/BasicPixelShader.hlsl", "BasicPS", "ps_5_0");
	//LoadHlsls::createPipeline(device, ShaderNo::PMD);

	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;	// �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	result = D3DCompileFromFile(//VS
		L"Resources/shaders/BasicVertexShader.hlsl",//�V�F�[�_�[��
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS", "vs_5_0",//�֐��A�ΏۃV�F�[�_�[
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
			return 0;//exit()
		}
		else {
			string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());//�f�[�^��\��
		}
	}

	result = D3DCompileFromFile(//PS
		L"Resources/shaders/BasicPixelShader.hlsl",//�V�F�[�_�[��
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS", "ps_5_0",//�֐��A�ΏۃV�F�[�_�[
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
			return 0;//exit()
		}
		else {
			string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());//�f�[�^��\��
		}
	}
#pragma endregion

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{//���_���W
		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//�@��
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//uv
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//�{�[���ԍ�
		"BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//�E�F�C�g
		"WEIGHT",0,DXGI_FORMAT_R8_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//�֊s���t���O
		"EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//�T���v���}�X�N,���X�^���C�U�[
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�f�t�H���g�̃T���v���}�X�N��\���萔
	gpipeline.RasterizerState.MultisampleEnable = false;//�A���`�G�C���A�X�i�܂��g��Ȃ��j
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//���g��h��Ԃ�
	gpipeline.RasterizerState.DepthClipEnable = true;//�[�x�����̃N���b�s���O�L��

	//�����_�^�[�Q�b�g�̐ݒ�
	gpipeline.NumRenderTargets = 1;//����1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0�`1�ɐ��K�����ꂽRGBA

	//�[�x�X�e���V��
	gpipeline.DepthStencilState.DepthEnable = true;//�g��
	gpipeline.DepthStencilState.StencilEnable = false;//
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//��������
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//�����������̗p
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//�u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlenddesc = {};
	renderTargetBlenddesc.BlendEnable = false;
	renderTargetBlenddesc.LogicOpEnable = false;
	renderTargetBlenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	gpipeline.BlendState.RenderTarget[0] = renderTargetBlenddesc;

	//���̓��C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;//���C�A�E�g�̐擪�A�h���X
	gpipeline.InputLayout.NumElements = _countof(inputLayout);//���C�A�E�g�z��̗v�f
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//�J�b�g�Ȃ�
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//�O�p�`�ō\��

	//�A���`�G�C���A�V���O�̂��߂̃T���v�����ݒ�
	gpipeline.SampleDesc.Count = 1;
	gpipeline.SampleDesc.Quality = 0;

	//rootSigBlob->Release();
	gpipeline.pRootSignature = _rootsignature.Get();

	//�O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐���
	//ID3D12PipelineState* _pipelinestate = nullptr;
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(_pipelinestate.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	return S_OK;
}

HRESULT PMDobject::CreateRootSignaturePMD()
{
	//�����W
	CD3DX12_DESCRIPTOR_RANGE  descTblRanges[4] = {};//�e�N�X�`���ƒ萔�̂Q��
	descTblRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//�萔[b0](�r���[�v���W�F�N�V�����p)
	descTblRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);//�萔[b1](���[���h�A�{�[���p)
	descTblRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);//�萔[b2](�}�e���A���p)
	descTblRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);//�e�N�X�`���S��(��{��sph��spa�ƃg�D�[��)

	//���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootParams[3] = {};
	rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

	rootParams[0].InitAsDescriptorTable(1, &descTblRanges[0]);//�r���[�v���W�F�N�V�����ϊ�
	rootParams[1].InitAsDescriptorTable(1, &descTblRanges[1]);//���[���h�E�{�[���ϊ�
	rootParams[2].InitAsDescriptorTable(2, &descTblRanges[2]);//�}�e���A������

	CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};
	samplerDescs[0].Init(0);
	samplerDescs[1].Init(1, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(3, rootParams, 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	auto result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob, &errorBlob);
	if (FAILED(result)) {
		assert(SUCCEEDED(result));
		return result;
	}
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(_rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(SUCCEEDED(result));
		return result;
	}
	return result;
}

//HRESULT PMDobject::CreateSceneView()
//{
//	HRESULT result;
//
//	result = device->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0_1) + 0xff) & ~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(_sceneConstBuff.ReleaseAndGetAddressOf())
//	);
//	if (FAILED(result)) {
//		assert(SUCCEEDED(result));
//		return result;
//	}
//
//	_mappedSceneData = nullptr;//�}�b�v��������|�C���^
//	result = _sceneConstBuff->Map(0, nullptr, (void**)&_mappedSceneData);//�}�b�v
//
//	const XMMATRIX& matViewProjection = dx12->Camera()->GetViewProjectionMatrix();
//	const XMFLOAT3& cameraPos = dx12->Camera()->GetEye();
//
//	_mappedSceneData->viewproj = matViewProjection;
//	_mappedSceneData->cameraPos = cameraPos;
//
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
//	descHeapDesc.NodeMask = 0;//�}�X�N��0
//	descHeapDesc.NumDescriptors = 1;//
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�f�X�N���v�^�q�[�v���
//	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(_sceneDescHeap.ReleaseAndGetAddressOf()));//����
//
//	////�f�X�N���v�^�̐擪�n���h�����擾���Ă���
//	auto heapHandle = _sceneDescHeap->GetCPUDescriptorHandleForHeapStart();
//
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
//	cbvDesc.BufferLocation = _sceneConstBuff->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = _sceneConstBuff->GetDesc().Width;
//	//�萔�o�b�t�@�r���[�̍쐬
//	device->CreateConstantBufferView(&cbvDesc, heapHandle);
//	return result;
//}