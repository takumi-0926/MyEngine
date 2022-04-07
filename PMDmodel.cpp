#include "PMDmodel.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>

#include<fstream>
#include<sstream>
#include<vector>
#include "pmdObject3D.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

//�ÓI�����o�̎���
ID3D12Device* PMDmodel::device = nullptr;
//�f�X�N���v�^�T�C�Y
UINT PMDmodel::descriptorHandleIncrementSize;
//�R�}���h���X�g
//ID3D12GraphicsCommandList* PMDmodel::cmdList = nullptr;
//�f�X�N���v�^�q�[�v
ComPtr<ID3D12DescriptorHeap> PMDmodel::descHeap;

ComPtr<ID3D12DescriptorHeap> PMDmodel::materialDescHeap;

//PMD�w�b�_
PMDmodel::PMDHeader PMDmodel::pmdheader;

TexMetadata PMDmodel::metadata;

ScratchImage PMDmodel::scratchImg;

ComPtr<ID3D12Resource> PMDmodel::LoadTextureFromFile(string& texPath) {

	HRESULT result;

	result = LoadFromWICFile(
		GetWideStringFromString(texPath).c_str(),
		WIC_FLAGS_NONE, &metadata, scratchImg);
	if (FAILED(result))return nullptr;
	auto img = scratchImg.GetImage(0, 0, 0);

	//WriteSubResource�œ]�����邽�߂̃q�[�v�ݒ�()
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC texresDesc = {};
	texresDesc.Format = metadata.format;
	texresDesc.Width = metadata.width;
	texresDesc.Height = metadata.height;
	texresDesc.DepthOrArraySize = metadata.arraySize;
	texresDesc.SampleDesc.Count = 1;
	texresDesc.SampleDesc.Quality = 0;
	texresDesc.MipLevels = metadata.mipLevels;
	texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	texresDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;//
	texresDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//�e�N�X�`���o�b�t�@�̃��\�[�X����
	result = device->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texbuff));
	if (FAILED(result))return nullptr;

	result = texbuff->WriteToSubresource(
		0, nullptr,
		img->pixels,
		img->rowPitch,
		img->slicePitch);
	if (FAILED(result))return nullptr;

	return texbuff;

}
ComPtr<ID3D12Resource> PMDmodel::CreateWhiteTexture()
{
	D3D12_HEAP_PROPERTIES texHeapProp = {};

	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	resDesc.Width = 4;
	resDesc.Height = 4;
	resDesc.DepthOrArraySize = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.MipLevels = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ComPtr<ID3D12Resource> whiteBuff = nullptr;
	auto result = device->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&whiteBuff));
	if (FAILED(result))return nullptr;

	vector<unsigned char>data(4 * 4 * 4);
	fill(data.begin(), data.end(), 0xff);//�S��255�Ŗ��߂�

	//�f�[�^�]��
	result = whiteBuff->WriteToSubresource(
		0,
		nullptr,
		data.data(),
		4 * 4,
		data.size());

	return  whiteBuff;
}
ComPtr<ID3D12Resource> PMDmodel::CreateBlackTexture()
{
	D3D12_HEAP_PROPERTIES texHeapProp = {};

	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	resDesc.Width = 4;
	resDesc.Height = 4;
	resDesc.DepthOrArraySize = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.MipLevels = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ComPtr<ID3D12Resource> blackBuff = nullptr;
	auto result = device->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&blackBuff));
	if (FAILED(result))return nullptr;

	vector<unsigned char>data(4 * 4 * 4);
	fill(data.begin(), data.end(), 0x00);//�S��255�Ŗ��߂�

	//�f�[�^�]��
	result = blackBuff->WriteToSubresource(
		0,
		nullptr,
		data.data(),
		4 * 4,
		data.size());

	return  blackBuff;
}

PMDmodel* PMDmodel::Create() 
{
	PMDmodel* pModel = new PMDmodel();

	return pModel;
}

bool PMDmodel::StaticInitialize(ID3D12Device* device)
{
	// �ď������`�F�b�N
	assert(!PMDmodel::device);

	// nullptr�`�F�b�N
	assert(device);

	PMDmodel::device = device;

	// �f�X�N���v�^�q�[�v�̏�����
	InitializeDescriptorHeap();

	return true;
}

bool PMDmodel::Initialize()
{
	// nullptr�`�F�b�N
	assert(device);

	//HRESULT result;
	//// �萔�o�b�t�@�̐���
	//result = _dev->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
	//	D3D12_HEAP_FLAG_NONE,
	//	&CD3DX12_RESOURCE_DESC::Buffer((sizeof(MatricesData) + 0xff) & ~0xff),
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&PMDconstBuffB1));

	//�}�e���A���o�b�t�@�̍쐬
	auto materiaBuffSize = sizeof(MaterialForHlsl);
	materiaBuffSize = (materiaBuffSize + 0xff) & ~0xff;

	HRESULT result;

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(materiaBuffSize * materialNum),//�����������������Ȃ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&PMDconstBuffB0));
	if (FAILED(result)) {
		assert(0);
	}

	return true;
}

void PMDmodel::Update()
{
	MaterialForHlsl* mapmatrix = nullptr;
	HRESULT result;

	result = PMDconstBuffB0->Map(0, nullptr, (void**)&mapmatrix);//�}�b�v
	if (FAILED(result)) {
		assert(0);
	}

	//�f�[�^�]��
	mapmatrix->alpha = material.material.alpha;
	mapmatrix->ambient = material.material.ambient;
	mapmatrix->diffuse = material.material.diffuse;
	mapmatrix->specular = material.material.specular;
	mapmatrix->specularStrength = material.material.specularStrength;
	PMDconstBuffB0->Unmap(0, nullptr);
}

void PMDmodel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptr�`�F�b�N
	assert(device);
	assert(cmdList);

	//cmdList->SetGraphicsRootSignature(rootsignature);
	cmdList->SetGraphicsRootConstantBufferView(0, PMDconstBuffB0->GetGPUVirtualAddress());//�{���̂����ł͂Ȃ���[
	//_cmdList->SetDescriptorHeaps(1, &basicDescHeap);
	//_cmdList->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

	ID3D12DescriptorHeap* mdh[] = { materialDescHeap.Get() };
	cmdList->SetDescriptorHeaps(1, mdh);
	//auto materialH = materialDescHeap->GetGPUDescriptorHandleForHeapStart();
	//unsigned int idxOffset = 0;
	//auto cbvsrvIncSize = _dev->GetDescriptorHandleIncrementSize(
	//	D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 4;
	//for (auto& m : materials) {

	//	cmdList->SetGraphicsRootDescriptorTable(1, materialH);
	//	cmdList->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);

	//	//�q�[�v�|�C���^�[�ƃC���f�b�N�X�����ɐi�߂�
	//	materialH.ptr += cbvsrvIncSize;
	//	idxOffset += m.indicesNum;
	//}
}

void PMDmodel::CreateModel(const std::string& strModelPath)
{
	HRESULT result;
	FILE* fp;
	char signature[3] = {};//�V�O�l�`��
	//string strModelPath = "Model/�����~�Nmetal.pmd";
	result = fopen_s(&fp, strModelPath.c_str(), "rb");//�t�@�C�����J��
	if (FAILED(result)) {
		assert(0);
	}

	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	unsigned int vertnum;//���_��
	fread(&vertnum, sizeof(vertnum), 1, fp);

	constexpr size_t pmdvertex_size = 38;//���_������̃T�C�Y
	vector<unsigned char>vertices(vertnum * pmdvertex_size);//�o�b�t�@�̊m��
	fread(vertices.data(), vertices.size(), 1, fp);//�ǂݍ���

#pragma region ���_�o�b�t�@
//���_�o�b�t�@�[�̐��� : WriteSubResource�œ]�����邽�߂̃q�[�v�ݒ�()
	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resdesc = {};
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.Width = sizeof(vertices);//
	resdesc.Height = 1;//
	resdesc.DepthOrArraySize = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.MipLevels = 1;
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;//
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;//
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//���_�o�b�t�@�̃��\�[�X����
	//ID3D12Resource* vertbuff = nullptr;
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertices.size()),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
	}

	//���_���̃R�s�[// 4/1
	unsigned char* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	vertBuff->Unmap(0, nullptr);//�}�b�v����

	//D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertices.size();
	vbView.StrideInBytes = pmdvertex_size;
#pragma endregion

	vector<unsigned short> indices;
	unsigned int indicesnum;
	fread(&indicesnum, sizeof(indicesnum), 1, fp);

	indices.resize(indicesnum);
	fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);

#pragma region �C���f�b�N�X�̎���
	//ID3D12Resource* idxBuff = nullptr;
	resdesc.Width = sizeof(indices);
	result = device->CreateCommittedResource(
		//&heapprop,
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		//&resdesc,
		&CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0])),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	if (FAILED(result)) {
		assert(0);
	}

	unsigned short* mappedIdx = nullptr;
	indexBuff->Map(0, nullptr, (void**)&mappedIdx);
	copy(begin(indices), end(indices), mappedIdx);
	indexBuff->Unmap(0, nullptr);

	//D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = indices.size() * sizeof(indices[0]);
#pragma endregion

	//�}�e���A���̓ǂݍ���
	fread(&materialNum, sizeof(materialNum), 1, fp);

	pmdMaterials.resize(materialNum);
	//vector<PMDMaterial>pmdMaterials(materialNum);
	fread(pmdMaterials.data(), pmdMaterials.size() * sizeof(PMDMaterial), 1, fp);

	//vector<ID3D12Resource*> textureResources(materialNum);
	//vector<ID3D12Resource*> sphResources(materialNum);
	//vector<ID3D12Resource*> spaResources(materialNum);
	textureResources.resize(materialNum);
	sphResources.resize(materialNum);
	spaResources.resize(materialNum);
	for (int i = 0; i < pmdMaterials.size(); ++i) {
		//if (strlen(pmdMaterials[i].texFilePath) == 0)
		//{
		//	textureResources[i] = nullptr;
		//}
		string texFileName = pmdMaterials[i].texFilePath;
		string sphFileName = "";
		if (count(texFileName.begin(), texFileName.end(), '*') > 0)
		{
			auto namepair = SplitFileName(texFileName);
			if (GetExtension(namepair.first) == "sph" || GetExtension(namepair.first) == "spa")
			{
				texFileName = namepair.second;
				//sphFileName = namepair.first;
			}
			else
				texFileName = namepair.first;
		}
		else {
			if (GetExtension(pmdMaterials[i].texFilePath) == "sph") {
				sphFileName = pmdMaterials[i].texFilePath;
				texFileName = "";
			}
			//else if (GetExtension(pmdMaterials[i].texFilePath) == "spa") {
			//	spaFileName = pmdMaterials[i].texFilePath;
			//	texFileName = "";
			//}
			else {
				texFileName = pmdMaterials[i].texFilePath;
			}
		}

		//���f���ƃe�N�X�`���p�X����A�v���P�[�V��������̃e�N�X�`���p�X�𓾂�
		if (texFileName != "") {
			auto texFilePath = GetTexturePathFromModelAndTexPath(strModelPath, texFileName.c_str());
			textureResources[i] = LoadTextureFromFile(texFilePath);
		}
		if (sphFileName != "") {
			auto sphFilePath = GetTexturePathFromModelAndTexPath(strModelPath, sphFileName.c_str());
			sphResources[i] = LoadTextureFromFile(sphFilePath);
		}
	}

	//vector<Material>materials(pmdMaterials.size());
	materials.resize(pmdMaterials.size());
	//�R�s�[
	for (int i = 0; i < pmdMaterials.size(); i++) {
		materials[i].indicesNum = pmdMaterials[i].indicesNum;
		materials[i].material.diffuse = pmdMaterials[i].diffuse;
		materials[i].material.alpha = pmdMaterials[i].alpha;
		materials[i].material.specular = pmdMaterials[i].specular;
		materials[i].material.specularStrength = pmdMaterials[i].specularStrength;
		materials[i].material.ambient = pmdMaterials[i].ambient;
	}
	//�}�e���A���o�b�t�@�̍쐬
	auto materiaBuffSize = sizeof(MaterialForHlsl);
	materiaBuffSize = (materiaBuffSize + 0xff) & ~0xff;

	ID3D12Resource* materialBuff = nullptr;
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(materiaBuffSize * materialNum),//�����������������Ȃ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&materialBuff));
	if (FAILED(result)) {
		assert(0);
	}

	//�}�b�v�}�e���A���ɃR�s�[
	char* mapMaterial = nullptr;
	result = materialBuff->Map(0, nullptr, (void**)&mapMaterial);

	for (auto& m : materials) {
		*((MaterialForHlsl*)mapMaterial) = m.material;//�f�[�^�R�s�[
		mapMaterial += materiaBuffSize;//���̃A���C�����g�ʒu�܂�
	}
	materialBuff->Unmap(0, nullptr);

	D3D12_DESCRIPTOR_HEAP_DESC matDescHeapDesc = {};
	matDescHeapDesc.NumDescriptors = materialNum * 4;
	matDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	matDescHeapDesc.NodeMask = 0;

	matDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	result = device->CreateDescriptorHeap(
		&matDescHeapDesc, IID_PPV_ARGS(&materialDescHeap));
	if (FAILED(result)) {
		assert(0);
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC matCBVDesc = {};
	matCBVDesc.BufferLocation = materialBuff->GetGPUVirtualAddress();
	matCBVDesc.SizeInBytes = materiaBuffSize;

	//�ʏ�e�N�X�`���r���[�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//RGBA(���K��)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;//
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;//�~�b�v�}�b�v�͎g�p���Ȃ�

	//�擪���L�^
	//auto cpuDescHandleSRV = materialDescHeap->GetCPUDescriptorHandleForHeapStart();
	cpuDescHandleSRV = materialDescHeap->GetCPUDescriptorHandleForHeapStart();
	gpuDescHandleSRV = materialDescHeap->GetGPUDescriptorHandleForHeapStart();

	auto inc = device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (int i = 0; i < materialNum; ++i) {
		//�}�e���A���p�萔�o�b�t�@�r���[
		device->CreateConstantBufferView(&matCBVDesc, cpuDescHandleSRV);
		cpuDescHandleSRV.ptr += inc;
		matCBVDesc.BufferLocation += materiaBuffSize;

		//�V�F�[�_�[���\�[�X�r���[
		if (textureResources[i] == nullptr) {
			srvDesc.Format = CreateWhiteTexture()->GetDesc().Format;
			device->CreateShaderResourceView(
				CreateWhiteTexture().Get(), &srvDesc, cpuDescHandleSRV);
		}
		else {
			srvDesc.Format = textureResources[i]->GetDesc().Format;
			device->CreateShaderResourceView(
				textureResources[i].Get(), &srvDesc, cpuDescHandleSRV);
		}
		cpuDescHandleSRV.ptr += inc;

		//��Z�X�t�B�A�}�b�v�p�r���[
		if (sphResources[i] == nullptr) {
			srvDesc.Format = CreateWhiteTexture()->GetDesc().Format;
			device->CreateShaderResourceView(
				CreateWhiteTexture().Get(), &srvDesc, cpuDescHandleSRV);
		}
		else {
			srvDesc.Format = sphResources[i]->GetDesc().Format;
			device->CreateShaderResourceView(
				sphResources[i].Get(), &srvDesc, cpuDescHandleSRV);
		}
		cpuDescHandleSRV.ptr += inc;

		//���Z�X�t�B�A�}�b�v�p�r���[
		if (spaResources[i] == nullptr) {
			srvDesc.Format = CreateBlackTexture()->GetDesc().Format;
			device->CreateShaderResourceView(
				CreateBlackTexture().Get(), &srvDesc, cpuDescHandleSRV);
		}
		else {
			srvDesc.Format = spaResources[i]->GetDesc().Format;
			device->CreateShaderResourceView(
				spaResources[i].Get(), &srvDesc, cpuDescHandleSRV);
		}
		cpuDescHandleSRV.ptr += inc;

	}

	fclose(fp);//�t�@�C�������
}

bool PMDmodel::InitializeDescriptorHeap()
{
	HRESULT result = S_FALSE;

	// �f�X�N���v�^�q�[�v�𐶐�	
	D3D12_DESCRIPTOR_HEAP_DESC descheapDesc = {};
	descheapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descheapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descheapDesc.NodeMask = 0;
	descheapDesc.NumDescriptors = 2;
	result = device->CreateDescriptorHeap(&descheapDesc, IID_PPV_ARGS(&descHeap));//����
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �f�X�N���v�^�T�C�Y���擾
	descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return true;
}

PMDmodel::PMDmodel()
{
}

//bool PMDmodel::InitializeGraphicsPipeline()
//{
//#pragma region hlsl�t�@�C���̃��[�h
//	ID3DBlob* vsBlob = nullptr;
//	ID3DBlob* psBlob = nullptr;
//	ID3DBlob* errorBlob = nullptr;
//
//	HRESULT result;
//
//	result = D3DCompileFromFile(//VS
//		L"Resources/shaders/BasicVertexShader.hlsl",//�V�F�[�_�[��
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		"BasicVS", "vs_5_0",//�֐��A�ΏۃV�F�[�_�[
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
//		0,
//		&vsBlob, &errorBlob);
//	if (FAILED(result)) {
//		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
//			::OutputDebugStringA("�t�@�C������������܂���");
//			return 0;//exit()
//		}
//		else {
//			string errstr;
//			errstr.resize(errorBlob->GetBufferSize());
//			copy_n((char*)errorBlob->GetBufferPointer(),
//				errorBlob->GetBufferSize(),
//				errstr.begin());
//			errstr += "\n";
//			::OutputDebugStringA(errstr.c_str());//�f�[�^��\��
//		}
//	}
//
//	result = D3DCompileFromFile(//PS
//		L"Resources/shaders/BasicPixelShader.hlsl",//�V�F�[�_�[��
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		"BasicPS", "ps_5_0",//�֐��A�ΏۃV�F�[�_�[
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
//		0,
//		&psBlob, &errorBlob);
//	if (FAILED(result)) {
//		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
//			::OutputDebugStringA("�t�@�C������������܂���");
//			return 0;//exit()
//		}
//		else {
//			string errstr;
//			errstr.resize(errorBlob->GetBufferSize());
//			copy_n((char*)errorBlob->GetBufferPointer(),
//				errorBlob->GetBufferSize(),
//				errstr.begin());
//			errstr += "\n";
//			::OutputDebugStringA(errstr.c_str());//�f�[�^��\��
//		}
//	}
//#pragma endregion
//
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
//	{//���_���W
//		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	{//�@��
//		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	{//uv
//		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	{//�{�[���ԍ�
//		"BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	{//�E�F�C�g
//		"WEIGHT",0,DXGI_FORMAT_R8_UINT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	{//�֊s���t���O
//		"EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	};
//
//#pragma region �p�C�v���C��
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
//	gpipeline.pRootSignature = nullptr;//��Őݒ�
//
//	//�V�F�[�_�[
//	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
//	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
//	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
//	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();
//
//	//�T���v���}�X�N,���X�^���C�U�[
//	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�f�t�H���g�̃T���v���}�X�N��\���萔
//	gpipeline.RasterizerState.MultisampleEnable = false;//�A���`�G�C���A�X�i�܂��g��Ȃ��j
//	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
//	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//���g��h��Ԃ�
//	gpipeline.RasterizerState.DepthClipEnable = true;//�[�x�����̃N���b�s���O�L��
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
//	gpipeline.BlendState.AlphaToCoverageEnable = false;
//	gpipeline.BlendState.IndependentBlendEnable = false;
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
//#pragma endregion
//
//#pragma region ���[�g�V�O�l�`��
//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
//	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//#pragma region �f�B�X�N���v�^�����W_P.206
//	D3D12_DESCRIPTOR_RANGE descTblRange[3] = {};
//	descTblRange[0].NumDescriptors = 1;//�萔1��
//	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//���(�萔)
//	descTblRange[0].BaseShaderRegister = 0;//�O�ԃX���b�g����
//	descTblRange[0].OffsetInDescriptorsFromTableStart
//		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//	descTblRange[1].NumDescriptors = 1;//�萔2��
//	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//���(�萔)
//	descTblRange[1].BaseShaderRegister = 1;//1�ԃX���b�g����
//	descTblRange[1].OffsetInDescriptorsFromTableStart
//		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//	descTblRange[2].NumDescriptors = 3;//�e�N�X�`��2��
//	descTblRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//���(�e�N�X�`��)
//	descTblRange[2].BaseShaderRegister = 0;//�O�ԃX���b�g����
//	descTblRange[2].OffsetInDescriptorsFromTableStart
//		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//#pragma endregion
//
//#pragma region ���[�g�p�����[�^�[_P.207
//	D3D12_ROOT_PARAMETER rootparam[2] = {};
//	//rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	//rootparam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
//	//rootparam[0].DescriptorTable.NumDescriptorRanges = 1;
//	//rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
//
//	//�{���̂����ł͂Ȃ���[
//	rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//	rootparam[0].Descriptor.RegisterSpace = 0;
//	rootparam[0].Descriptor.ShaderRegister = 0;
//	rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
//
//	rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	rootparam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];
//	rootparam[1].DescriptorTable.NumDescriptorRanges = 2;
//	rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
//
//#pragma endregion
//
//	rootSignatureDesc.pParameters = rootparam;//���[�g�p�����[�^�[�̐擪�A�h���X
//	rootSignatureDesc.NumParameters = 2;//���[�g�p�����[�^�[��
//
//#pragma region �T���v���[
//	D3D12_STATIC_SAMPLER_DESC sampleDesc = {};
//	sampleDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�������̌J��Ԃ�
//	sampleDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�����̌J��Ԃ�
//	sampleDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s���̌J��Ԃ�
//	sampleDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�͍�
//	sampleDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;//���`���
//	sampleDesc.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
//	sampleDesc.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
//	sampleDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_���猩����
//	sampleDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//���T���v�����O���Ȃ�
//#pragma endregion
//
//	rootSignatureDesc.pStaticSamplers = &sampleDesc;
//	rootSignatureDesc.NumStaticSamplers = 1;
//
//	ID3DBlob* rootSigBlob = nullptr;
//	result = D3D12SerializeRootSignature(
//		&rootSignatureDesc,
//		D3D_ROOT_SIGNATURE_VERSION_1_0,
//		&rootSigBlob,
//		&errorBlob);
//
//	result = _dev->CreateRootSignature(
//		0,
//		rootSigBlob->GetBufferPointer(),
//		rootSigBlob->GetBufferSize(),
//		IID_PPV_ARGS(&rootsignature));
//	if (FAILED(result)) {
//		return result;
//	}
//
//	rootSigBlob->Release();
//	gpipeline.pRootSignature = rootsignature;
//
//	//�O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐���
//	ID3D12PipelineState* _pipelinestate = nullptr;
//	result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&_pipelinestate));
//	if (FAILED(result)) {
//		return result;
//	}
//#pragma endregion 
//
//	return true;
//}


//void PMDmodel::preDraw(ID3D12GraphicsCommandList* cmdList)
//{
//	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
//	assert(PMDmodel::cmdList == nullptr);
//
//	// �R�}���h���X�g���Z�b�g
//	PMDmodel::cmdList = cmdList;
//
//	// �v���~�e�B�u�`���ݒ�
//	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//}


//void PMDmodel::postDraw()
//{
//	// �R�}���h���X�g������
//	PMDmodel::cmdList = nullptr;
//}


