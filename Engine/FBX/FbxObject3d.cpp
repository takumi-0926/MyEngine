#include "FbxObject3d.h"
#include "FbxLoader.h"
#include "..\pipelineSet.h"
#include "Collision\BaseCollision.h"
#include "Collision\CollisionManager.h"

#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

ID3D12Device* FbxObject3d::device = nullptr;
Camera* FbxObject3d::camera = nullptr;
ComPtr<ID3D12RootSignature> FbxObject3d::rootsignature;
ComPtr<ID3D12PipelineState> FbxObject3d::pipelinestate;

void FbxObject3d::Initialize()
{
	HRESULT result;

	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);

	CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDetaTransform) + 0xff) & ~0xff);
	result = device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBufferTransform)
	);

	desc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff);
	result = device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin)
	);
}

void FbxObject3d::Update()
{
	UpdateWorldMatrix();

	const XMMATRIX& matViewProjection =
		camera->GetViewProjectionMatrix();

	const XMMATRIX& modelTransform =
		model->GetModelTransform();

	const XMFLOAT3& cameraPos =
		camera->GetEye();

	HRESULT result;

	ConstBufferDetaTransform* constMap = nullptr;
	result = constBufferTransform->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->viewproj = matViewProjection;
		constMap->world = modelTransform * matWorld;
		constMap->cameraPos = cameraPos;
		constBufferTransform->Unmap(0, nullptr);
	}

	std::vector<FbxModel::Bone>& bones = model->GetBones();

	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < bones.size(); i++)
	{
		XMMATRIX matCurrentPose;

		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);

		FbxLoader::ConvertMatrixFormFbx(&matCurrentPose, fbxCurrentPose);

		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}

	constBuffSkin->Unmap(0, nullptr);

	if (isPlay)
	{
		currentTime += frameTime;
		if (currentTime > animas[nowPlayMotion].endTime)
		{
			currentTime = animas[nowPlayMotion].startTime;
		}
	}
}

void FbxObject3d::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (model == nullptr) {
		return;
	}

	cmdList->SetPipelineState(LoadHlsls::pipeline.at(ShaderNo::FBX)._pipelinestate.Get());

	cmdList->SetGraphicsRootSignature(LoadHlsls::pipeline.at(ShaderNo::FBX)._rootsignature.Get());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->SetGraphicsRootConstantBufferView(
		0, constBufferTransform->GetGPUVirtualAddress()
	);

	cmdList->SetGraphicsRootConstantBufferView(
		3, constBuffSkin->GetGPUVirtualAddress()
	);

	model->Draw(cmdList);
}

void FbxObject3d::UpdateWorldMatrix()
{
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	if (!useRotMat) {
		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
		matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
		matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	}
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matWorld = XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;
}

void FbxObject3d::PlayAnimation(int playNum)
{
	//if (isPlay) { return; }

	nowPlayMotion = playNum;
	FbxScene* fbxScene = model->GetFbxScene();

	fbxScene->SetCurrentAnimationStack(animas[nowPlayMotion].stack);

	animas[nowPlayMotion].startTime = animas[nowPlayMotion].info->mLocalTimeSpan.GetStart();

	animas[nowPlayMotion].endTime = animas[nowPlayMotion].info->mLocalTimeSpan.GetStop();

	currentTime = animas[nowPlayMotion].startTime;

	isPlay = true;
}

void FbxObject3d::LoadAnima()
{
	FbxScene* fbxScene;
	fbxScene = model->GetFbxScene();
	animas.clear();

	//�A�j���[�V�����̌���ۑ�
	int AnimaStackNum = fbxScene->GetSrcObjectCount<FbxAnimStack>();

	for (int i = 0; i < AnimaStackNum; i++) {
		//FbxAnimStack* animStack;
		//animStack = fbxScene->GetSrcObject<FbxAnimStack>(i);

		AnimationInfelno instance;

		instance.stack = fbxScene->GetSrcObject<FbxAnimStack>(i);

		instance.Name = instance.stack->GetName();

		instance.info = fbxScene->GetTakeInfo(instance.stack->GetName());

		instance.startTime = instance.info->mLocalTimeSpan.GetStart();

		instance.endTime = instance.info->mLocalTimeSpan.GetStop();

		animas.push_back(instance);
	}
}

void FbxObject3d::CreateGraphicsPipeline()
{
	//�p�C�v���C������
	LoadHlsls::LoadHlsl_VS(ShaderNo::FBX, L"Resources/shaders/FBXVS.hlsl", "main", "vs_5_0");
	LoadHlsls::LoadHlsl_PS(ShaderNo::FBX, L"Resources/shaders/FBXPS.hlsl", "main", "ps_5_0");
	LoadHlsls::createPipeline(device, ShaderNo::FBX);

	//HRESULT result = S_FALSE;
	//ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	//ComPtr<ID3DBlob> psBlob;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	//ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	//assert(device);

	//// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	//result = D3DCompileFromFile(
	//	L"Resources/shaders/FBXVS.hlsl",    // �V�F�[�_�t�@�C����
	//	nullptr,
	//	D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
	//	"main", "vs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
	//	0,
	//	&vsBlob, &errorBlob);
	//if (FAILED(result)) {
	//	// errorBlob����G���[���e��string�^�ɃR�s�[
	//	std::string errstr;
	//	errstr.resize(errorBlob->GetBufferSize());

	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferSize(),
	//		errstr.begin());
	//	errstr += "\n";
	//	// �G���[���e���o�̓E�B���h�E�ɕ\��
	//	OutputDebugStringA(errstr.c_str());
	//	exit(1);
	//}

	//// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	//result = D3DCompileFromFile(
	//	L"Resources/shaders/FBXPS.hlsl",    // �V�F�[�_�t�@�C����
	//	nullptr,
	//	D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
	//	"main", "ps_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
	//	0,
	//	&psBlob, &errorBlob);
	//if (FAILED(result)) {
	//	// errorBlob����G���[���e��string�^�ɃR�s�[
	//	std::string errstr;
	//	errstr.resize(errorBlob->GetBufferSize());

	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferSize(),
	//		errstr.begin());
	//	errstr += "\n";
	//	// �G���[���e���o�̓E�B���h�E�ɕ\��
	//	OutputDebugStringA(errstr.c_str());
	//	exit(1);
	//}

	//// ���_���C�A�E�g
	//D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	//	{ // xy���W(1�s�ŏ������ق������₷��)
	//		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
	//		"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // uv���W(1�s�ŏ������ق������₷��)
	//		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
	//		"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
	//		"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//};

	//// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	//gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	//gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//// �T���v���}�X�N
	//gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	//// ���X�^���C�U�X�e�[�g
	//gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	////gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	////gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//// �f�v�X�X�e���V���X�e�[�g
	//gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	//D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	//blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA�S�Ẵ`�����l����`��
	//blenddesc.BlendEnable = true;
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	//blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	//// �u�����h�X�e�[�g�̐ݒ�
	//gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//// �[�x�o�b�t�@�̃t�H�[�}�b�g
	//gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//// ���_���C�A�E�g�̐ݒ�
	//gpipeline.InputLayout.pInputElementDescs = inputLayout;
	//gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//// �}�`�̌`��ݒ�i�O�p�`�j
	//gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//gpipeline.NumRenderTargets = 1;    // �`��Ώۂ�1��
	//gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA
	//gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	//// �f�X�N���v�^�����W
	//CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	//descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	//// ���[�g�p�����[�^
	//CD3DX12_ROOT_PARAMETER rootparams[3];
	//// CBV�i���W�ϊ��s��p�j
	//rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//// SRV�i�e�N�X�`���j
	//rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	////CBV�i�X�L�j���O�j
	//rootparams[2].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	//// �X�^�e�B�b�N�T���v���[
	//CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//// ���[�g�V�O�l�`���̐ݒ�
	//CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	//rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//ComPtr<ID3DBlob> rootSigBlob;
	//// �o�[�W������������̃V���A���C�Y
	//result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//// ���[�g�V�O�l�`���̐���
	//result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootsignature.ReleaseAndGetAddressOf()));
	//if (FAILED(result)) { assert(0); }

	//gpipeline.pRootSignature = rootsignature.Get();

	//// �O���t�B�b�N�X�p�C�v���C���̐���
	//result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate.ReleaseAndGetAddressOf()));
	//if (FAILED(result)) { assert(0); }
}

void FbxObject3d::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	CollisionManager::GetInstance()->AddCollider(collider);
	UpdateWorldMatrix();
	collider->Update();
}
