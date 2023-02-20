#include "PostEffect.h"
#include "application.h"
//#include <d3dx12.h>
#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

//using namespace DirectX;
//
const float PostEffect::clearColor[4] = { 0.5f,0.5f,0.5f,0.5f };

//���_��
const int VertNum = 4;

PostEffect::PostEffect()
	:Sprite(
		100,
		{ 0.0f,0.0f },
		{ 500.0f,500.0f },
		{ 1,1,1,1 },
		{ 0.0f,0.0f },
		false,
		false)
{
}

void PostEffect::Initialize()
{
	//Sprite::Initalize();

	HRESULT result;
	
		CD3DX12_HEAP_PROPERTIES vertProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC VertDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * VertNum);
	
		//���_�o�b�t�@���쐬
		result = device->CreateCommittedResource(
			&vertProperties,
			D3D12_HEAP_FLAG_NONE,
			&VertDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(_vertbuff.ReleaseAndGetAddressOf()));
	//
	//	////�w��ԍ��̉摜���ǂݍ��ݍς݂Ȃ�
	//	//if (spritecommon._texBuff[texNumber]) {
	//	//	D3D12_RESOURCE_DESC resDesc = spritecommon._texBuff[texNumber]->GetDesc();
	//
	//	//	size = { (float)resDesc.Width,(float)resDesc.Height };
	//	//}
	
		//���_�o�b�t�@�Ƀf�[�^��]��
		VertexPosUv vertices[VertNum] = {
			{{-1.0f,-1.0f,0.0f},{0.0f,1.0f}},
			{{-1.0f,+1.0f,0.0f},{0.0f,0.0f}},
			{{+1.0f,-1.0f,0.0f},{1.0f,1.0f}},
			{{+1.0f,+1.0f,0.0f},{1.0f,0.0f}},
		};
		VertexPosUv* vertMap = nullptr;
		result = _vertbuff->Map(0, nullptr, (void**)&vertMap);
		if (SUCCEEDED(result)) {
			memcpy(vertMap, vertices, sizeof(vertices));
			_vertbuff->Unmap(0, nullptr);
		}
	
		//���_�o�b�t�@�r���[�̍쐬
		vbView.BufferLocation = _vertbuff->GetGPUVirtualAddress();
		vbView.SizeInBytes = sizeof(VertexPosUv) * 4;
		vbView.StrideInBytes = sizeof(VertexPosUv);
	
		CD3DX12_HEAP_PROPERTIES constProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC constDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);
	
		//�萔�o�b�t�@�̐���
		result = device->CreateCommittedResource(
			&constProperties,
			D3D12_HEAP_FLAG_NONE,
			&constDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(constBuff.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(result));
	//
	//	////�萔�o�b�t�@�Ƀf�[�^��]��
	//	//ConstBufferData* constMap = nullptr;
	//	//result = constBuff->Map(0, nullptr, (void**)&constMap);
	//	//constMap->color = this->color;
	//	////constMap->mat = spritecommon.matProjection;
	//	//constMap->mat = XMMatrixIdentity();
	//	//constBuff->Unmap(0, nullptr);


	//�e�N�X�`�����\�[�X�̐ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		Application::window_width,
		(UINT)Application::window_height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	//�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(
			D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor),
		IID_PPV_ARGS(&texBuff)
	);
	assert(SUCCEEDED(result));

	//auto texProperties = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	//auto texValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor);

	////�o�b�t�@����
	//result = device->CreateCommittedResource(
	//	&texProperties,
	//	D3D12_HEAP_FLAG_NONE,
	//	&texresDesc,
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
	//	&texValue,
	//	IID_PPV_ARGS(&texBuff)
	//);
	//assert(SUCCEEDED(result));
//
	//��f��
	const UINT pixelCount = Application::window_width * Application::window_height;
	//��s���̃f�[�^�T�C�Y
	const UINT rowPitch = sizeof(UINT) * Application::window_width;
	//�S�̂̃f�[�^�T�C�Y
	const UINT depthPitch = rowPitch * Application::window_height;

	//�摜�C���[�W
	UINT* img = new UINT[pixelCount];
	for (int i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }

	//�f�[�^�]��
	result = texBuff->WriteToSubresource(
		0, nullptr, img, rowPitch, depthPitch);
	assert(SUCCEEDED(result));
	delete[] img;
//
	//SRV�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeap = {};
	srvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeap.NumDescriptors = 1;

	result = device->CreateDescriptorHeap(
		&srvDescHeap, IID_PPV_ARGS(descHeapSRV.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	//SRV�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//SRV�쐬
	device->CreateShaderResourceView(
		texBuff.Get(),
		&srvDesc,
		descHeapSRV->GetCPUDescriptorHandleForHeapStart());

	//RTV�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeap = {};
	rtvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeap.NumDescriptors = 1;

	result = device->CreateDescriptorHeap(
		&rtvDescHeap, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));

	//�����_�[�^�[�Q�b�g�r���[�̐���
	device->CreateRenderTargetView(
		texBuff.Get(),
		nullptr,
		descHeapRTV->GetCPUDescriptorHandleForHeapStart());

	//�[�x�o�b�t�@�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			Application::window_width,
			Application::window_height,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	auto depthProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto depthValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	result = device->CreateCommittedResource(
		&depthProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthValue,
		IID_PPV_ARGS(&depthBuff));
	assert(SUCCEEDED(result));

	//DSV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeap{};
	dsvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescHeap.NumDescriptors = 1;

	result = device->CreateDescriptorHeap(
		&dsvDescHeap,
		IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));

	//�f�v�X�X�e���V���r���[�̍쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,
		descHeapDSV->GetCPUDescriptorHandleForHeapStart());
//
//	CreateGraphicsPipeline();
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList) {
	matWorld = XMMatrixIdentity();

	matWorld *= XMMatrixRotationZ(XMConvertToRadians(rotation));

	matWorld *= XMMatrixTranslation(position.x, position.y, 0);

	ConstBufferData* constMap = nullptr;

	this->color.w = this->alpha;

	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	//constMap->mat = matWorld * spritecommon.matProjection;
	constMap->mat = XMMatrixIdentity();
	constMap->color = this->color;
	constBuff->Unmap(0, nullptr);

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelineset._pipelinestate.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipelineset._rootsignature.Get());
	// �v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ���_�o�b�t�@�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &this->vbView);

	//ID3D12DescriptorHeap* ppHeaps[] = { spritecommon._descHeap.Get() };
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get()};
	// �f�X�N���v�^�q�[�v���Z�b�g
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, this->constBuff->GetGPUVirtualAddress());
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	//cmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(spritecommon._descHeap->GetGPUDescriptorHandleForHeapStart(), this->texNumber, descriptorHandleIncrementSize));
	cmdList->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());
	// �`��R�}���h
	cmdList->DrawInstanced(4, 1, 0, 0);



	//// ���_�o�b�t�@�̐ݒ�
	//cmdList->IASetVertexBuffers(0, 1, &this->vbView);

	//ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	//// �f�X�N���v�^�q�[�v���Z�b�g
	//cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//// �萔�o�b�t�@�r���[���Z�b�g
	//cmdList->SetGraphicsRootConstantBufferView(0, this->constBuff->GetGPUVirtualAddress());
	//// �V�F�[�_���\�[�X�r���[���Z�b�g
	////cmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(spritecommon._descHeap->GetGPUDescriptorHandleForHeapStart(), this->texNumber, descriptorHandleIncrementSize));
	//// �`��R�}���h
	//cmdList->DrawInstanced(4, 1, 0, 0);
}

void PostEffect::PreDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	//���\�[�X�o���A�̕ύX
	CD3DX12_RESOURCE_BARRIER barrier = 
		CD3DX12_RESOURCE_BARRIER::Transition(
			texBuff.Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
			D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList->ResourceBarrier(1, &barrier);

	//RTV�p�f�X�N���v�^�q�[�v�̃n���h���擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
		descHeapRTV->GetCPUDescriptorHandleForHeapStart();

	//DSV�p�f�X�N���v�^�q�[�v�̃n���h���擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV->GetCPUDescriptorHandleForHeapStart();

	//�����_�^�[�Q�b�g�̃Z�b�g
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	//�r���[�|�[�g
	CD3DX12_VIEWPORT viewport = 
		CD3DX12_VIEWPORT(
			0.0f, 0.0f, 
			Application::window_width, 
			Application::window_height);
	cmdList->RSSetViewports(1, &viewport);

	//�V�U�����O
	CD3DX12_RECT rect = 
		CD3DX12_RECT(0, 0, 
			Application::window_width, 
			Application::window_height);
	cmdList->RSSetScissorRects(1, &rect);

	//��ʃN���A
	cmdList->ClearRenderTargetView(
		rtvH,
		clearColor,
		0,
		nullptr);

	//�[�x�o�b�t�@�N���A
	cmdList->ClearDepthStencilView(
		dsvH, D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr);
}

void PostEffect::PostDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	//���\�[�X�o���A�ύX�i�`�� -> �V�F�[�_�[���\�[�X�j
	CD3DX12_RESOURCE_BARRIER barrier = 
		CD3DX12_RESOURCE_BARRIER::Transition(
			texBuff.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);
}

void PostEffect::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;	// �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/PostEffectVS.hlsl",	// �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/PostEffectPS.hlsl",	// �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy���W(1�s�ŏ������ق������₷��)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv���W(1�s�ŏ������ق������₷��)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; // ��ɏ㏑�����[��

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[2] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT); // s0 ���W�X�^

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, 
		&samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;

	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	// ���[�g�V�O�l�`���̐���
	result = device->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&pipelineset._rootsignature));
	assert(SUCCEEDED(result));

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = pipelineset._rootsignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = device->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&pipelineset._pipelinestate));
	assert(SUCCEEDED(result));
}
