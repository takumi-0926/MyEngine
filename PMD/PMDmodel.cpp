#include "PMDmodel.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include<fstream>
#include<sstream>
#include <algorithm>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib,"winmm.lib")

#include "..\object\baseObject.h"
#include "..\Application\dx12Wrapper.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

//�ÓI�����o�̎���
ID3D12Device* PMDmodel::device = nullptr;

ComPtr<ID3D12DescriptorHeap> PMDmodel::materialDescHeap;

namespace {
	//���f���̃p�X�ƃe�N�X�`���̃p�X���獇���p�X�𓾂�
	//@param modelpath �A�v���P�[�V�������猩��pmd���f���̃p�X
	//@param texPath PMD ���f�����猩���e�N�X�`���̃p�X
	//@return �A�v���P�[�V�������猩���e�N�X�`���̃p�X
	static string GetTexturePathFromModelAndTexPath(const string& modelPath, const char* texPath) {
		int pathIndex1 = modelPath.rfind('/');
		int pathIndex2 = modelPath.rfind('\\');

		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}

	//std::string{�}���`�o�C�g������}����std::wstring{���C�h������}�𓾂�
	//@param str �}���`�o�C�g������
	//@return �ϊ����ꂽ������
	static std::wstring GetWideStringFromString(const string& str)
	{
		//�Ăяo��1���(�����񐔂𓾂�)
		auto num1 = MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1, nullptr, 0);

		std::wstring wstr;//string��wchar_t��
		wstr.resize(num1);//����ꂽ�����񐔂Ń��T�C�Y

		//�Ăяo��2���(�m�ۍς݂�wstr�ɕϊ���������R�s�[)
		auto num2 = MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1, &wstr[0], num1);

		assert(num1 == num2);//�`�F�b�N
		return  wstr;
	}

	//�t�@�C��������g���q���擾����
	//@param path �Ώۂ̃p�X������
	//@return �g���q
	static string GetExtension(const string& path) {
		int idx = path.rfind('.');
		return path.substr(idx + 1, path.length() - idx - 1);
	}

	//�e�N�X�`���̃p�X���Z�p���[�^�[�����Ŏ擾����
	//@param path �Ώۂ̃p�X������
	//@param splitter ��؂蕶��
	//@return �����O��̕�����y�A
	static std::pair<string, string>SplitFileName(const string& path, const char splitter = '*') {
		int idx = path.find(splitter);
		std::pair<string, string>ret;
		ret.first = path.substr(0, idx);
		ret.second = path.substr((idx + 1), path.length() - idx - 1);

		return ret;
	}
}

ComPtr<ID3D12Resource> PMDmodel::LoadTextureFromFile(string& texPath) {

	HRESULT result;
	TexMetadata metadata = {};
	ScratchImage scratchImg = {};

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
ID3D12Resource* PMDmodel::CreateWhiteTexture()
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

	ID3D12Resource* whiteBuff = nullptr;
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

	return whiteBuff;
}
ID3D12Resource* PMDmodel::CreateBlackTexture()
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

	ID3D12Resource* blackBuff = nullptr;
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

	return blackBuff;
}

void* PMDmodel::Transform::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

PMDmodel::PMDmodel(Wrapper* _dx12, const char* filepath, PMDobject& object) :
	_object(object)
{
	// �ď������`�F�b�N
	assert(!PMDmodel::device);

	// nullptr�`�F�b�N
	assert(_dx12->GetDevice());

	PMDmodel::device = _dx12->GetDevice();

	PMDmodel::dx12 = _dx12;

	transform.world = XMMatrixIdentity();
	LoadPMDFile(filepath);
	CreateDescHeap();
	CreateTransform();
	LoadVMDFile(vmdData::WAIT, "Resources/vmd/marieru_stand.vmd");
	LoadVMDFile(vmdData::WALK, "Resources/vmd/Rick�����胂�[�V����02.vmd");
	CreateMaterial();
	CreateMaterialAndTextureView();
}

PMDmodel::~PMDmodel()
{
}

void PMDmodel::Update()
{
	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	XMMATRIX matScale, matTrans;
	HRESULT result;
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//matRot = XMMatrixIdentity();
	//matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	//matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	//matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	result = transformBuff->Map(0, nullptr, (void**)&_mappedMatrices);
	if (FAILED(result)) { assert(0); }
	_mappedMatrices[0] = matWorld;
	copy(boneMatrices.begin(), boneMatrices.end(), _mappedMatrices + 1);

	transformBuff->Unmap(0, nullptr);

	MotionUpdate();
}

void PMDmodel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptr�`�F�b�N
	assert(dx12->GetDevice());
	assert(cmdList);

	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	auto heapHandle = dx12->GetDescHeap()->GetGPUDescriptorHandleForHeapStart();
	auto IncSize = dx12->GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	heapHandle.ptr += IncSize;
	cmdList->SetGraphicsRootDescriptorTable(1, heapHandle);

	//ID3D12DescriptorHeap* mdh[] = { materialDescHeap.Get() };
	//cmdList->SetDescriptorHeaps(1, mdh);

	heapHandle.ptr += IncSize;
	unsigned int idxOffset = 0;
	auto cbvsrvIncSize = dx12->GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 4;
	for (auto& m : materials) {

		cmdList->SetGraphicsRootDescriptorTable(2, heapHandle);
		cmdList->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);

		//�q�[�v�|�C���^�[�ƃC���f�b�N�X�����ɐi�߂�
		heapHandle.ptr += cbvsrvIncSize;
		idxOffset += m.indicesNum;
	}
}

HRESULT PMDmodel::LoadPMDFile(const char* path)
{
	//*�g�p�\����*----------------------------------------
#pragma pack(1)//����̍�
	//�}�e���A���\���́i�r���Ŏg��Ȃ��Ȃ�j
	struct PMDMaterial {
		XMFLOAT3 diffuse;//�f�B�t���[�Y�F
		float alpha;	//�f�B�t���[�Y��
		float specularStrength;//�X�y�L�����̋����i��Z�l�j
		XMFLOAT3 specular;//�X�y�L�����F
		XMFLOAT3 ambient;//�A���r�G���g�F
		unsigned char toonIdx;//�g�D�[���ԍ�
		unsigned char edgeFlg;//�}�e���A�����Ƃ̗֊s���t���O

		//�Q�o�C�g�̃p�f�B���O����

		uint32_t indicesNum;//���̃}�e���A�������蓖�Ă���

		char texFilePath[20];//�e�N�X�`���t�@�C���p�X+��

	};//�v�V�O�o�C�g�����A�Q�o�C�g�̃p�f�B���O�����邽�߂V�Q�o�C�g�ɂȂ�
#pragma pack()
	//�w�b�_�[
	struct PMDHeader {
		float vertion;
		char model_name[20];
		char comment[256];
	};

	struct PMDVertex {
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
		unsigned short boneNo[2];
		unsigned char boneWeight;
		unsigned char edgeFlg;
	};

	//*�t�@�C���I�[�v��*----------------------------------
	HRESULT result;
	FILE* fp = nullptr;
	PMDHeader pmdheader = {};

	string modelPath = path;

	char signature[3] = {};//�V�O�l�`��
	//string strModelPath = "Model/�����~�Nmetal.pmd";
	result = fopen_s(&fp, modelPath.c_str(), "rb");//�t�@�C�����J��
	if (FAILED(result)) { assert(0); }

	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	//*���_�֘A*------------------------------------------
	uint32_t vertnum;//���_��
	fread(&vertnum, sizeof(vertnum), 1, fp);
	constexpr unsigned int pmdvertex_size = sizeof(PMDVertex) - 2;//���_������̃T�C�Y
	std::vector<unsigned char> vertices(vertnum * pmdvertex_size);// ���_�f�[�^�z��
	vertices.resize(vertnum * pmdvertex_size);//�o�b�t�@�̊m��
	fread(vertices.data(), vertices.size(), 1, fp);//�ǂݍ���

	//���_�o�b�t�@�[�̐���
	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	D3D12_RESOURCE_DESC resdesc = {};
	resdesc = CD3DX12_RESOURCE_DESC::Buffer(
		vertices.size() * sizeof(vertices[0]));

	//���_�o�b�t�@�̃��\�[�X����
	result = device->CreateCommittedResource(
		&heapprop, D3D12_HEAP_FLAG_NONE,
		&resdesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	//���_���̃R�s�[
	unsigned char* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	vertBuff->Unmap(0, nullptr);//�}�b�v����

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertices.size();
	vbView.StrideInBytes = pmdvertex_size;

	//*���_�C���f�b�N�X�֘A*------------------------------
	uint32_t indicesnum;
	fread(&indicesnum, sizeof(indicesnum), 1, fp);
	std::vector<unsigned short> indices(indicesnum);// ���_�C���f�b�N�X�z��	
	fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);

	resdesc = CD3DX12_RESOURCE_DESC::Buffer(
		indices.size() * sizeof(indices[0]));

	result = device->CreateCommittedResource(
		&heapprop, D3D12_HEAP_FLAG_NONE,
		&resdesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&indexBuff));
	if (FAILED(result)) { assert(0); }

	unsigned short* mappedIdx = nullptr;
	indexBuff->Map(0, nullptr, (void**)&mappedIdx);
	copy(begin(indices), end(indices), mappedIdx);
	indexBuff->Unmap(0, nullptr);

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = indices.size() * sizeof(indices[0]);

	//*�}�e���A���֘A*------------------------------------
//�}�e���A���̓ǂݍ���
	fread(&materialNum, sizeof(materialNum), 1, fp);

	materials.resize(materialNum);
	textureResources.resize(materialNum);
	sphResources.resize(materialNum);
	spaResources.resize(materialNum);

	std::vector<PMDMaterial>pmdMaterials(materialNum);
	fread(pmdMaterials.data(), pmdMaterials.size() * sizeof(PMDMaterial), 1, fp);
	//�R�s�[
	for (int i = 0; i < pmdMaterials.size(); i++) {
		materials[i].indicesNum = pmdMaterials[i].indicesNum;
		materials[i].material.diffuse = pmdMaterials[i].diffuse;
		materials[i].material.alpha = pmdMaterials[i].alpha;
		materials[i].material.specular = pmdMaterials[i].specular;
		materials[i].material.specularStrength = pmdMaterials[i].specularStrength;
		materials[i].material.ambient = pmdMaterials[i].ambient;
	}

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
			auto texFilePath = GetTexturePathFromModelAndTexPath(modelPath, texFileName.c_str());
			textureResources[i] = LoadTextureFromFile(texFilePath);
		}
		if (sphFileName != "") {
			auto sphFilePath = GetTexturePathFromModelAndTexPath(modelPath, sphFileName.c_str());
			sphResources[i] = LoadTextureFromFile(sphFilePath);
		}
	}

	CreateBone(fp);

	struct PMDIK {
		uint16_t boneIdx;
		uint16_t targetidx;
		uint16_t iterations;
		float limit;
		std::vector<uint16_t> nodeIdx;
	};

	uint16_t ikNum = 0;
	fread(&ikNum, sizeof(ikNum), 1, fp);

	std::vector<PMDIK> pmdIkData(ikNum);

	for (auto& ik : pmdIkData) {
		fread(&ik.boneIdx, sizeof(ik.boneIdx), 1, fp);
		fread(&ik.targetidx, sizeof(ik.targetidx), 1, fp);

		uint8_t chainLen = 0;

		fread(&chainLen, sizeof(chainLen), 1, fp);
		ik.nodeIdx.resize(chainLen);
		fread(&ik.iterations, sizeof(ik.iterations), 1, fp);
		fread(&ik.limit, sizeof(ik.limit), 1, fp);

		if (chainLen == 0) {
			continue;
		}

		fread(ik.nodeIdx.data(), sizeof(ik.nodeIdx[0]), chainLen, fp);
	}

	fclose(fp);

	return S_OK;
}

HRESULT PMDmodel::LoadVMDFile(const unsigned int Number, const char* path)
{
	struct VMDMotion {
		char boneName[15];
		unsigned int frameNo;
		XMFLOAT3 location;
		XMFLOAT4 puaternion;
		unsigned char bezier[64];
	};

	//struct Motion {
	//	unsigned int frameNo;
	//	XMVECTOR quaternion;

	//	Motion(unsigned int fno, XMVECTOR& q) :
	//		frameNo(fno), quaternion(q) {
	//	}
	//};

	//*�t�@�C���I�[�v��*----------------------------------
	vmdMotion data;
	HRESULT result;
	FILE* fp = nullptr;
	string modelPath = path;

	result = fopen_s(&fp, modelPath.c_str(), "rb");//�t�@�C�����J��
	if (FAILED(result)) { assert(0); }

	fseek(fp, 50, SEEK_SET);
	unsigned int motionDataNum = 0;
	fread(&motionDataNum, sizeof(motionDataNum), 1, fp);

	vector<VMDMotion> vmdMotionData(motionDataNum);

	for (auto& motion : vmdMotionData)
	{
		fread(motion.boneName, sizeof(motion.boneName), 1, fp);
		fread(&motion.frameNo, sizeof(motion.frameNo) + sizeof(motion.location) + sizeof(motion.puaternion) + sizeof(motion.bezier), 1, fp);
		data.duration = std::max<unsigned int>(data.duration, motion.frameNo);
	}

	//unordered_map<string, vector<Motion>> _motionData;
	for (auto& vmdMotion : vmdMotionData) {
		data._motionData[vmdMotion.boneName].emplace_back(
			Motion(
				vmdMotion.frameNo,
				DirectX::XMLoadFloat4(&vmdMotion.puaternion),
				XMFLOAT2((float)vmdMotion.bezier[3] / 127.0f, (float)vmdMotion.bezier[7] / 127.0f),
				XMFLOAT2((float)vmdMotion.bezier[11] / 127.0f, (float)vmdMotion.bezier[15] / 127.0f)
			));

	}

	for (auto& boneMoiton : data._motionData) {
		auto node = _boneNodeTable[boneMoiton.first];
		auto& pos = node.startPos;
		auto mat =
			XMMatrixTranslation(-pos.x, -pos.y, -pos.z)
			* XMMatrixRotationQuaternion(boneMoiton.second[0].quaternion)
			* XMMatrixTranslation(pos.x, pos.y, pos.z);
		boneMatrices[node.boneIdx] = mat;
	}

	//�\�[�g
	for (auto& motion : data._motionData) {
		std::sort(
			motion.second.begin(),
			motion.second.end(),
			[](const Motion& lval, const Motion& rval) {
				return lval.frameNo <= rval.frameNo;
			});
	}

	recursiveMatrixMultiply(&_boneNodeTable["�Z���^�["], XMMatrixIdentity());
	copy(boneMatrices.begin(), boneMatrices.end(), _mappedMatrices + 1);

	motion.insert(std::make_pair(Number, data));

	return S_OK;
}

HRESULT PMDmodel::CreateTransform()
{
	auto buffSize = sizeof(XMMATRIX) * (1 + boneMatrices.size());
	buffSize = (buffSize + 0xff) & ~0xff;

	auto result = dx12->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(buffSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&transformBuff));
	if (FAILED(result)) { assert(0); }

	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	transform.world = matWorld;
	result = transformBuff->Map(0, nullptr, (void**)&_mappedMatrices);
	if (FAILED(result)) { assert(0); }
	//*mappedTransform = transform;
	_mappedMatrices[0] = matWorld;
	copy(boneMatrices.begin(), boneMatrices.end(), _mappedMatrices + 1);

	auto heapHandle = dx12->GetDescHeap()->GetCPUDescriptorHandleForHeapStart();
	auto IncSize = dx12->GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	heapHandle.ptr += IncSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = transformBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = buffSize;
	dx12->GetDevice()->CreateConstantBufferView(
		&cbvDesc,
		heapHandle);

	return S_OK;
}

HRESULT PMDmodel::CreateMaterial()
{
	auto materiaBuffSize = sizeof(MaterialForHlsl);
	materiaBuffSize = (materiaBuffSize + 0xff) & ~0xff;

	auto result = dx12->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(materiaBuffSize * materialNum),//�����������������Ȃ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&materialBuff));
	if (FAILED(result)) { assert(0); }

	//�}�b�v�}�e���A���ɃR�s�[
	char* mapMaterial = nullptr;
	result = materialBuff->Map(0, nullptr, (void**)&mapMaterial);
	if (FAILED(result)) { assert(0); }

	for (auto& m : materials) {
		*((MaterialForHlsl*)mapMaterial) = m.material;//�f�[�^�R�s�[
		mapMaterial += materiaBuffSize;//���̃A���C�����g�ʒu�܂�
	}
	materialBuff->Unmap(0, nullptr);

	return S_OK;
}

HRESULT PMDmodel::CreateBone(FILE* fp)
{
	//�{�[�����擾
	unsigned short boneNum = 0;
	fread(&boneNum, sizeof(boneNum), 1, fp);

	//�{�[���f�[�^�擾
	pmdBones.resize(boneNum);
	fread(pmdBones.data(), sizeof(PMDBone), boneNum, fp);

	vector<string> boneNames(pmdBones.size());

	//�{�[���m�[�h�}�b�v�쐬
	for (int idx = 0; idx < pmdBones.size(); idx++)
	{
		auto& pb = pmdBones[idx];
		boneNames[idx] = pb.boneName;
		auto& node = _boneNodeTable[pb.boneName];
		node.boneIdx = idx;
		node.startPos = pb.pos;
	}

	//�e�q�֌W���\�z
	for (auto& pb : pmdBones)
	{
		if (pb.parentNo >= pmdBones.size())
		{
			continue;
		}

		auto parentName = boneNames[pb.parentNo];
		_boneNodeTable[parentName].children.emplace_back(&_boneNodeTable[pb.boneName]);
	}

	boneMatrices.resize(pmdBones.size());

	//�{�[����������
	std::fill(
		boneMatrices.begin(),
		boneMatrices.end(),
		XMMatrixIdentity());



	return S_OK;
}

HRESULT PMDmodel::CreateMaterialAndTextureView()
{
	auto materiaBuffSize = sizeof(MaterialForHlsl);
	materiaBuffSize = (materiaBuffSize + 0xff) & ~0xff;

	D3D12_DESCRIPTOR_HEAP_DESC matDescHeapDesc = {};
	matDescHeapDesc.NumDescriptors = materialNum * 4;
	matDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	matDescHeapDesc.NodeMask = 0;

	matDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	auto result = dx12->GetDevice()->CreateDescriptorHeap(
		&matDescHeapDesc, IID_PPV_ARGS(&materialDescHeap));
	if (FAILED(result)) { assert(0); }

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
	auto matDescHeapH = dx12->GetDescHeap()->GetCPUDescriptorHandleForHeapStart();
	auto inc = dx12->GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	matDescHeapH.ptr += inc;
	matDescHeapH.ptr += inc;
	for (int i = 0; i < materialNum; ++i) {
		//�}�e���A���p�萔�o�b�t�@�r���[
		dx12->GetDevice()->CreateConstantBufferView(&matCBVDesc, matDescHeapH);
		matDescHeapH.ptr += inc;
		matCBVDesc.BufferLocation += materiaBuffSize;

		//�V�F�[�_�[���\�[�X�r���[
		if (textureResources[i] == nullptr) {
			srvDesc.Format = CreateWhiteTexture()->GetDesc().Format;
			dx12->GetDevice()->CreateShaderResourceView(
				CreateWhiteTexture(), &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = textureResources[i]->GetDesc().Format;
			dx12->GetDevice()->CreateShaderResourceView(
				textureResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += inc;

		//��Z�X�t�B�A�}�b�v�p�r���[
		if (sphResources[i] == nullptr) {
			srvDesc.Format = CreateWhiteTexture()->GetDesc().Format;
			dx12->GetDevice()->CreateShaderResourceView(
				CreateWhiteTexture(), &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = sphResources[i]->GetDesc().Format;
			dx12->GetDevice()->CreateShaderResourceView(
				sphResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += inc;

		//���Z�X�t�B�A�}�b�v�p�r���[
		if (spaResources[i] == nullptr) {
			srvDesc.Format = CreateBlackTexture()->GetDesc().Format;
			dx12->GetDevice()->CreateShaderResourceView(
				CreateBlackTexture(), &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = spaResources[i]->GetDesc().Format;
			dx12->GetDevice()->CreateShaderResourceView(
				spaResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += inc;

	}

	return S_OK;
}

HRESULT PMDmodel::CreateDescHeap() {
	//�f�B�X�N���v�^�q�[�v����(�ėp)
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�ǂ�ȃr���[�����̂�()
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1024;//�S�̂̃q�[�v�̈搔
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//���Ɏw��Ȃ�

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	auto result = dx12->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(descHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}
	return S_OK;
}

void PMDmodel::recursiveMatrixMultiply(BoneNode* node, const XMMATRIX& mat)
{
	boneMatrices[node->boneIdx] *= mat;

	for (auto& cnode : node->children)
	{
		recursiveMatrixMultiply(cnode, boneMatrices[node->boneIdx]);
	}
}

void PMDmodel::MotionUpdate()
{
	vmdMotion data = motion.at(vmdNumber);
	auto elapsedTime = timeGetTime() - _startTime;

	unsigned int frameNo = 30 * (elapsedTime / 1000.0f);

	if (frameNo > data.duration) {
		_startTime = timeGetTime();
		frameNo = 0;
	}

	if (vmdNumber != oldVmdNumber) {
		_startTime = timeGetTime();
		frameNo = 0;
	}

	std::fill(boneMatrices.begin(), boneMatrices.end(), XMMatrixIdentity());

	for (auto& boneMoiton : data._motionData) {
		auto node = _boneNodeTable[boneMoiton.first];

		auto motions = boneMoiton.second;
		auto rit = std::find_if(
			motions.rbegin(),//���o�[�X�C�e���[�^�[
			motions.rend(),//���o�[�X�C�e���[�^�[
			[frameNo](const Motion& motion) {
				return motion.frameNo <= frameNo;
			});

		if (rit == motions.rend())
		{
			continue;
		}

		auto& pos = node.startPos;
		//auto& pos = position;

		XMMATRIX rotation;
		auto it = rit.base();

		if (it != motions.end()) {
			auto t = static_cast<float>(frameNo - rit->frameNo)
				/ static_cast<float>(it->frameNo - rit->frameNo);

			if (a == true) {
				t = GetYFromXOn(t, it->p1, it->p2, 12);
			}

			rotation = XMMatrixRotationQuaternion(
				XMQuaternionSlerp(rit->quaternion, it->quaternion, t));//���ʐ��`���
		}
		else {
			rotation = XMMatrixRotationQuaternion(rit->quaternion);
		}

		auto mat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z)
			* rotation
			* XMMatrixTranslation(pos.x, pos.y, pos.z);

		boneMatrices[node.boneIdx] = mat;
	}

	recursiveMatrixMultiply(&_boneNodeTable["�Z���^�["], XMMatrixIdentity());
	copy(boneMatrices.begin(), boneMatrices.end(), _mappedMatrices + 1);
}

void PMDmodel::playAnimation()
{
	_startTime = timeGetTime();
}

float PMDmodel::GetYFromXOn(float x, const XMFLOAT2& a, const XMFLOAT2& b, uint8_t n)
{
	if (a.x == a.y && b.x == b.y) {
		return x;
	}

	float t = x;
	const float k0 = 1 + 3 * a.x - 3 * b.x;
	const float k1 = 3 * b.x - 6 * a.x;
	const float k2 = 3 * a.x;

	constexpr float epsilon = 0.0005f;

	for (int i = 0; i < n; ++i) {
		auto it = k0 * t * t * t + k1 * t * t + k2 * t - x;

		if (it <= epsilon && it >= -epsilon) {
			break;
		}

		t -= it / 2;
	}

	auto r = 1 - t;

	return t * t * t + 3 * t * t * r * b.y + 3 * t * r * r * a.y;
}
