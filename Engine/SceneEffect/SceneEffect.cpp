#include "SceneEffect.h"
#include <DirectXTex.h>

#include "dx12Wrapper.h"

Wrapper* SceneEffect::dx12 = nullptr;

const float SceneEffect::radius = 5.0f;				// ��ʂ̔��a
const float SceneEffect::prizmHeight = 8.0f;			// ���̍���
ID3D12Device* SceneEffect::device = nullptr;
UINT SceneEffect::descriptorHandleIncrementSize = 0;
ID3D12GraphicsCommandList* SceneEffect::cmdList = nullptr;
ComPtr<ID3D12RootSignature> SceneEffect::rootsignature;
ComPtr<ID3D12PipelineState> SceneEffect::pipelinestate;
ComPtr<ID3D12DescriptorHeap> SceneEffect::descHeap;
ComPtr<ID3D12Resource> SceneEffect::vertBuff;
//ComPtr<ID3D12Resource> Object3d::indexBuff;
ComPtr<ID3D12Resource> SceneEffect::texbuff;
CD3DX12_CPU_DESCRIPTOR_HANDLE SceneEffect::cpuDescHandleSRV;
CD3DX12_GPU_DESCRIPTOR_HANDLE SceneEffect::gpuDescHandleSRV;
XMMATRIX SceneEffect::matView{};
XMMATRIX SceneEffect::matProjection{};
XMMATRIX SceneEffect::matBillboard = XMMatrixIdentity();
XMMATRIX SceneEffect::matBillboardY = XMMatrixIdentity();
SpriteCommon SceneEffect::spritecommon;
Camera* SceneEffect::camera = nullptr;

bool SceneEffect::staticInitalize(ID3D12Device* _dev, SIZE ret)
{
	assert(_dev);
	SceneEffect::device = _dev;
	descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//�p�C�v���C������
	LoadHlsls::LoadHlsl_VS(ShaderNo::OBJ, L"Resources/shaders/OBJVertexShader.hlsl", "main", "vs_5_0");
	LoadHlsls::LoadHlsl_PS(ShaderNo::OBJ, L"Resources/shaders/OBJPixelShader.hlsl", "main", "ps_5_0");
	LoadHlsls::createPipeline(device, ShaderNo::OBJ);

	//�p�C�v���C������
	{
		HRESULT result = S_FALSE;
		ComPtr<ID3DBlob> vsBlob;	// ���_�V�F�[�_�I�u�W�F�N�g
		ComPtr<ID3DBlob> gsBlob;	// �W�I���g���V�F�[�_�I�u�W�F�N�g
		ComPtr<ID3DBlob> psBlob;	// �s�N�Z���V�F�[�_�I�u�W�F�N�g
		ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

		// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/shaders/ParticleVS.hlsl",	// �V�F�[�_�t�@�C����
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
			exit(1);
		}

		// �W�I���g���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/shaders/ParticleGS.hlsl",	// �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "gs_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&gsBlob, &errorBlob);
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
			exit(1);
		}

		// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/shaders/ParticlePS.hlsl",	// �V�F�[�_�t�@�C����
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
			exit(1);
		}

		// ���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{ // xy���W(1�s�ŏ������ق������₷��)
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			},
			//{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
			//	"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			//	D3D12_APPEND_ALIGNED_ELEMENT,
			//	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			//},
			{ // uv���W(1�s�ŏ������ق������₷��)
				"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			},
		};

		// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

		// �T���v���}�X�N
		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
		// ���X�^���C�U�X�e�[�g
		gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		// �f�v�X�X�e���V���X�e�[�g
		gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
		D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
		blenddesc.BlendEnable = true;
		//����������
		//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		//blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		//blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		//���Z����
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;

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
		//gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;


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
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		// ���[�g�V�O�l�`���̐ݒ�
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> rootSigBlob;
		// �o�[�W������������̃V���A���C�Y
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
		// ���[�g�V�O�l�`���̐���
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
		if (FAILED(result)) {
			return result;
		}

		gpipeline.pRootSignature = rootsignature.Get();

		// �O���t�B�b�N�X�p�C�v���C���̐���
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));

		if (FAILED(result)) {
			return result;
		}
	}

	return true;
}

bool SceneEffect::loadTexture(UINT texNumber, const wchar_t* fileName)
{
	// nullptr�`�F�b�N
	assert(device);

	HRESULT result;
	// WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		fileName, WIC_FLAGS_NONE,
		&metadata, scratchImg);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	const Image* img = scratchImg.GetImage(0, 0, 0); // ���f�[�^���o

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	// �e�N�X�`���p�o�b�t�@�̐���
	CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	result = device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // �e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(&spritecommon._texBuff[texNumber]));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = spritecommon._texBuff[texNumber]->WriteToSubresource(
		0,
		nullptr, // �S�̈�փR�s�[
		img->pixels,    // ���f�[�^�A�h���X
		(UINT)img->rowPitch,  // 1���C���T�C�Y
		(UINT)img->slicePitch // 1���T�C�Y
	);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �V�F�[�_���\�[�X�r���[�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // �ݒ�\����
	D3D12_RESOURCE_DESC resDesc = spritecommon._texBuff[texNumber]->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(spritecommon._texBuff[texNumber].Get(), //�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc, //�e�N�X�`���ݒ���
		CD3DX12_CPU_DESCRIPTOR_HANDLE(spritecommon._descHeap->GetCPUDescriptorHandleForHeapStart(), texNumber, descriptorHandleIncrementSize)
	);

	return true;
}

void SceneEffect::PreDraw(ID3D12GraphicsCommandList* _cmdList)
{
	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
	assert(SceneEffect::cmdList == nullptr);

	// �R�}���h���X�g���Z�b�g
	SceneEffect::cmdList = cmdList;

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(LoadHlsls::pipeline.at(ShaderNo::OBJ)._pipelinestate.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(LoadHlsls::pipeline.at(ShaderNo::OBJ)._rootsignature.Get());
	// �v���~�e�B�u�`���ݒ�
	//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void SceneEffect::PostDraw()
{
	// �R�}���h���X�g������
	SceneEffect::cmdList = nullptr;
}

void SceneEffect::UpdateViewMatrix()
{
	XMVECTOR eyePosition = XMLoadFloat3(&camera->GetEye());
	XMVECTOR targetPosition = XMLoadFloat3(&camera->GetTarget());
	XMVECTOR upVector = XMLoadFloat3(&camera->GetUp());

	///�J������Z��(��������)
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	//�O�x�N�g�������O
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));
	//�x�N�g���𐳋K��
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	///�J������X��(�E����)
	XMVECTOR cameraAxisX;
	//X���͏������Z���̊O�ςŋ��߂�
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	//�x�N�g���𐳋K��
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	///�J������Y��(�����)
	XMVECTOR cameraAxisY;
	//Y����Z����X���̊O�ςŋ��߂�
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
	//�x�N�g���𐳋K��
	cameraAxisY = XMVector3Normalize(cameraAxisY);

	///�J������]�s��
	XMMATRIX matCameraRot;
	//�J�������W�n���烏�[���h���W�n�̕ϊ��s��
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	///�ʒu�ɂ��t�s����v�Z
	matView = XMMatrixTranspose(matCameraRot);

	//���_���W��-1���|�������W
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);
	//1�̃x�N�g���ɂ܂Ƃ߂�
	XMVECTOR transration = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[3], 1.0f);

	//�r���[�s��ɕ��s�ړ�������ݒ�
	matView.r[3] = transration;

#pragma region �S�����r���{�[�h
	//�r���{�[�h�s��
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma endregion

	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	ybillCameraAxisX = cameraAxisX;
	ybillCameraAxisY = XMVector3Normalize(upVector);
	ybillCameraAxisZ = XMVector3Cross(
		cameraAxisX, cameraAxisY);

	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);

}

SceneEffect* SceneEffect::Create(UINT texNumber, XMFLOAT3 position, XMFLOAT4 color, XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	// ���T�C�Y
	XMFLOAT2 size = { 100.0f, 100.0f };

	if (spritecommon._texBuff[texNumber])
	{
		// �e�N�X�`�����擾
		D3D12_RESOURCE_DESC resDesc = spritecommon._texBuff[texNumber]->GetDesc();
		// �X�v���C�g�̃T�C�Y���e�N�X�`���̃T�C�Y�ɐݒ�
		size = { (float)resDesc.Width, (float)resDesc.Height };
	}

	//�C���X�^���X�𐶐�
	SceneEffect* instance = new SceneEffect(texNumber, position, size, color, anchorpoint, isFlipX, isFlipY);
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	if (!instance->Initalize()) {
		delete instance;
		assert(0);
		return nullptr;
	}

	return instance;
}

SceneEffect::SceneEffect(UINT texNumber, XMFLOAT3 position, XMFLOAT2 size, XMFLOAT4 color, XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	this->position = position;
	this->size = size;
	this->anchorpoint = anchorpoint;
	this->matWorld = XMMatrixIdentity();
	this->color = color;
	this->texNumber = texNumber;
	this->isFlipX = isFlipX;
	this->isFlipY = isFlipY;
	this->texSize = size;
}

bool SceneEffect::Initalize()
{
	// nullptr�`�F�b�N
	assert(device);

	HRESULT result;
	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	return true;
}

void SceneEffect::Update()
{
	// ���_�o�b�t�@�փf�[�^�]��
	VertexPos* vertMap = nullptr;
	auto result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		for (std::forward_list<sceneEffect>::iterator it = effects.begin(); it != effects.end(); it++) {
			vertMap->pos = it->position;
			vertMap->scale = it->scale;
			vertMap++;
		}
		vertBuff->Unmap(0, nullptr);
	}
	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();

	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->matBillboard = matBillboard;
	constMap->mat = matViewProjection;
	if (SUCCEEDED(result)) {
		for (std::forward_list<sceneEffect>::iterator it = effects.begin(); it != effects.end(); it++) {
			constMap->color = it->color;
			//constMap++;
		}
	}
	constBuff->Unmap(0, nullptr);
}

void SceneEffect::Draw()
{
	// nullptr�`�F�b�N
	assert(device);
	assert(SceneEffect::cmdList);

	// ���_�o�b�t�@�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	//cmdList->IASetIndexBuffer(&ibView);

	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);
	// �`��R�}���h
	//cmdList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);
	//cmdList->DrawInstanced(_countof(vertices), 1, 0, 0);
	cmdList->DrawInstanced((UINT)std::distance(effects.begin(), effects.end()), 1, 0, 0);
}

void SceneEffect::CreateParticle(XMFLOAT3 emitter, XMFLOAT4 color)
{
	effects.emplace_front();

	sceneEffect& p = effects.front();

	p.position = emitter;
	p.scale = 100.0f;
	p.color = color;
}

