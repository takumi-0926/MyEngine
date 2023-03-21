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

	//アニメーションの個数を保存
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
	//パイプライン生成
	LoadHlsls::LoadHlsl_VS(ShaderNo::FBX, L"Resources/shaders/FBXVS.hlsl", "main", "vs_5_0");
	LoadHlsls::LoadHlsl_PS(ShaderNo::FBX, L"Resources/shaders/FBXPS.hlsl", "main", "ps_5_0");
	LoadHlsls::createPipeline(device, ShaderNo::FBX);
}

void FbxObject3d::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	CollisionManager::GetInstance()->AddCollider(collider);
	UpdateWorldMatrix();
	collider->Update();
}
