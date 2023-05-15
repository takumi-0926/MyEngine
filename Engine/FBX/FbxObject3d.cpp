#include "FbxObject3d.h"
#include "FbxLoader.h"
#include "..\pipelineSet.h"
#include "Collision\BaseCollision.h"
#include "Collision\CollisionManager.h"

#include "DebugImgui.h"

#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

ID3D12Device* FbxObject3d::device = nullptr;
Camera* FbxObject3d::camera = nullptr;
ComPtr<ID3D12RootSignature> FbxObject3d::rootsignature;
ComPtr<ID3D12PipelineState> FbxObject3d::pipelinestate;
ComPtr<ID3D12PipelineState> FbxObject3d::pipelineshadow;

void FbxObject3d::staticInitialize(ID3D12Device* _device)
{
	FbxObject3d::SetDevice(_device);
	FbxObject3d::CreateGraphicsPipeline();
}

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

	//���C�g�J����
	auto light = XMFLOAT4(1, -1, 0, 0);
	XMVECTOR lightVec = XMLoadFloat4(&light);

	auto Eye = XMFLOAT3(DebugImgui::shadowlightPos[0], DebugImgui::shadowlightPos[1], DebugImgui::shadowlightPos[2]);
	XMVECTOR eye = XMLoadFloat3(&Eye);
	auto Target = XMFLOAT3(DebugImgui::shadowlightTarget[0], DebugImgui::shadowlightTarget[1], DebugImgui::shadowlightTarget[2]);
	XMVECTOR terget = XMLoadFloat3(&Target);
	XMVECTOR up = XMLoadFloat3(&camera->GetUp());

	XMVECTOR lightPos = eye;

	ConstBufferDetaTransform* constMap = nullptr;
	result = constBufferTransform->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->viewproj = matViewProjection;
		constMap->lightCamera = XMMatrixLookAtLH(lightPos, terget, up) * XMMatrixOrthographicLH(DebugImgui::shadowCameraSite[0], DebugImgui::shadowCameraSite[1], DebugImgui::shadowlightLange[0], DebugImgui::shadowlightLange[1]);
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
			if (!isLoop) {
				currentTime = animas[nowPlayMotion].startTime;
				playEnd = true;
			}
		}
	}
}

void FbxObject3d::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (model == nullptr) {
		return;
	}

	cmdList->SetPipelineState(pipelinestate.Get());

	cmdList->SetGraphicsRootSignature(rootsignature.Get());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->SetGraphicsRootConstantBufferView(
		0, constBufferTransform->GetGPUVirtualAddress()
	);

	cmdList->SetGraphicsRootConstantBufferView(
		3, constBuffSkin->GetGPUVirtualAddress()
	);

	model->Draw(cmdList);
}

void FbxObject3d::ShadowDraw(ID3D12GraphicsCommandList* cmdList)
{
	if (model == nullptr) {
		return;
	}

	cmdList->SetPipelineState(pipelineshadow.Get());

	cmdList->SetGraphicsRootSignature(rootsignature.Get());

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
	nowPlayMotion = playNum;
	FbxScene* fbxScene = model->GetFbxScene();

	fbxScene->SetCurrentAnimationStack(animas[nowPlayMotion].stack);

	animas[nowPlayMotion].startTime = animas[nowPlayMotion].info->mLocalTimeSpan.GetStart();

	animas[nowPlayMotion].endTime = animas[nowPlayMotion].info->mLocalTimeSpan.GetStop();

	currentTime = animas[nowPlayMotion].startTime;

	isPlay = true;
}

void FbxObject3d::ChangeAnimation(int num, bool flag)
{
	if (nowPlayMotion != num) {
		PlayAnimation(num);
		nowPlayMotion = num;
		isLoop = flag;
	}
}

void FbxObject3d::StopAnimation()
{
	nowPlayMotion = -1;
	isPlay = false;
}

void FbxObject3d::LoadAnima()
{
	FbxScene* fbxScene;
	fbxScene = model->GetFbxScene();
	animas.clear();

	//�A�j���[�V�����̌���ۑ�
	int AnimaStackNum = fbxScene->GetSrcObjectCount<FbxAnimStack>();

	for (int i = 0; i < AnimaStackNum; i++) {

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
	//LoadHlsls::LoadHlsl_VS(ShaderNo::FBX, , "main", "vs_5_0");
	//LoadHlsls::LoadHlsl_PS(ShaderNo::FBX, L"Resources/shaders/FBXPS.hlsl", "main", "ps_5_0");
	//LoadHlsls::createPipeline(device, ShaderNo::FBX);

	HRESULT result;
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	ComPtr<ID3DBlob> rootSigBlob;
	//PipelineSet pipelineset;
	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//�V�F�[�_�[�ǂݍ���----------------------------------------
	{
		//�w��(namePath)��hlsl�t�@�C�������[�h
		result = D3DCompileFromFile(
			L"Resources/shaders/FBXVS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "vs_5_0",
			D3DCOMPILE_DEBUG,
			0,
			&vsBlob, &errorBlob);
		//�ǂݍ��ݐ����`�F�b�N
		if (FAILED(result)) {
			if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
				::OutputDebugStringA("�t�@�C������������܂���");
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
		//�w��(namePath)��hlsl�t�@�C�������[�h
		result = D3DCompileFromFile(
			L"Resources/shaders/FBXPS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "ps_5_0",
			D3DCOMPILE_DEBUG,
			0,
			&psBlob, &errorBlob);
		//�ǂݍ��ݐ����`�F�b�N
		if (FAILED(result)) {
			if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
				::OutputDebugStringA("�t�@�C������������܂���");
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
	}

	//�p�C�v���C������------------------------------------------------------------
#pragma region
		// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy���W(1�s�ŏ������ق������₷��)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv���W(1�s�ŏ������ق������₷��)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
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
	};
	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	CD3DX12_ROOT_SIGNATURE_DESC _rootSignatureDesc = {};


	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;    // �`��Ώۂ�1��
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV[2] = {};
	descRangeSRV[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^
	descRangeSRV[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[5] = {};
	// CBV�i���W�ϊ��s��p�j
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	// SRV�i�e�N�X�`���j
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV[0], D3D12_SHADER_VISIBILITY_ALL);
	//CBV�i�X�L�j���O�j
	rootparams[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	//�V���h�E�}�b�v
	rootparams[4].InitAsDescriptorTable(1, &descRangeSRV[1]);//

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};
	samplerDescs[0].Init(0);
	samplerDescs[1].Init(1, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	// ���[�g�V�O�l�`���̐���
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = rootsignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate.ReleaseAndGetAddressOf()));
#pragma endregion
	//�V���h�E�}�b�v�p���_�V�F�[�_�[�ǂݍ���--------------------------------------
#pragma region
	result = D3DCompileFromFile(//VS
		L"Resources/shaders/FbxShadowShader.hlsl",//�V�F�[�_�[��
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"shadowVS", "vs_5_0",//�֐��A�ΏۃV�F�[�_�[
		0,
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
			return;//exit()
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

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS.BytecodeLength = 0;
	gpipeline.PS.pShaderBytecode = nullptr;
	gpipeline.NumRenderTargets = 0;	// �`��Ώۂ�0��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;

	result = device->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(pipelineshadow.ReleaseAndGetAddressOf())
	);
#pragma endregion
}

void FbxObject3d::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	CollisionManager::GetInstance()->AddCollider(collider);
	UpdateWorldMatrix();
	collider->Update();
}
