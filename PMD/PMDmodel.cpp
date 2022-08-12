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

//静的メンバの実体
ID3D12Device* PMDmodel::device = nullptr;

ComPtr<ID3D12DescriptorHeap> PMDmodel::materialDescHeap;

namespace {
	//モデルのパスとテクスチャのパスから合成パスを得る
	//@param modelpath アプリケーションから見たpmdモデルのパス
	//@param texPath PMD モデルから見たテクスチャのパス
	//@return アプリケーションから見たテクスチャのパス
	static string GetTexturePathFromModelAndTexPath(const string& modelPath, const char* texPath) {
		int pathIndex1 = modelPath.rfind('/');
		int pathIndex2 = modelPath.rfind('\\');

		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}

	//std::string{マルチバイト文字列}からstd::wstring{ワイド文字列}を得る
	//@param str マルチバイト文字列
	//@return 変換された文字列
	static std::wstring GetWideStringFromString(const string& str)
	{
		//呼び出し1回目(文字列数を得る)
		auto num1 = MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1, nullptr, 0);

		std::wstring wstr;//stringのwchar_t版
		wstr.resize(num1);//得られた文字列数でリサイズ

		//呼び出し2回目(確保済みのwstrに変換文字列をコピー)
		auto num2 = MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1, &wstr[0], num1);

		assert(num1 == num2);//チェック
		return  wstr;
	}

	//ファイル名から拡張子を取得する
	//@param path 対象のパス文字列
	//@return 拡張子
	static string GetExtension(const string& path) {
		int idx = path.rfind('.');
		return path.substr(idx + 1, path.length() - idx - 1);
	}

	//テクスチャのパスをセパレーター文字で取得する
	//@param path 対象のパス文字列
	//@param splitter 区切り文字
	//@return 分離前後の文字列ペア
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

	//WriteSubResourceで転送するためのヒープ設定()
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

	//テクスチャバッファのリソース生成
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
	fill(data.begin(), data.end(), 0xff);//全部255で埋める

	//データ転送
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
	fill(data.begin(), data.end(), 0x00);//全部255で埋める

	//データ転送
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
	// 再初期化チェック
	assert(!PMDmodel::device);

	// nullptrチェック
	assert(_dx12->GetDevice());

	PMDmodel::device = _dx12->GetDevice();

	PMDmodel::dx12 = _dx12;

	transform.world = XMMatrixIdentity();
	LoadPMDFile(filepath);
	CreateDescHeap();
	CreateTransform();
	LoadVMDFile(vmdData::WAIT, "Resources/vmd/marieru_stand.vmd");
	LoadVMDFile(vmdData::WALK, "Resources/vmd/Rick式走りモーション02.vmd");
	CreateMaterial();
	CreateMaterialAndTextureView();
}

PMDmodel::~PMDmodel()
{
}

void PMDmodel::Update()
{
	// スケール、回転、平行移動行列の計算
	XMMATRIX matScale, matTrans;
	HRESULT result;
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//matRot = XMMatrixIdentity();
	//matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	//matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	//matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

	result = transformBuff->Map(0, nullptr, (void**)&_mappedMatrices);
	if (FAILED(result)) { assert(0); }
	_mappedMatrices[0] = matWorld;
	copy(boneMatrices.begin(), boneMatrices.end(), _mappedMatrices + 1);

	transformBuff->Unmap(0, nullptr);

	MotionUpdate();
}

void PMDmodel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptrチェック
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

		//ヒープポインターとインデックスを次に進める
		heapHandle.ptr += cbvsrvIncSize;
		idxOffset += m.indicesNum;
	}
}

HRESULT PMDmodel::LoadPMDFile(const char* path)
{
	//*使用構造体*----------------------------------------
#pragma pack(1)//苦肉の策
	//マテリアル構造体（途中で使わなくなる）
	struct PMDMaterial {
		XMFLOAT3 diffuse;//ディフューズ色
		float alpha;	//ディフューズα
		float specularStrength;//スペキュラの強さ（乗算値）
		XMFLOAT3 specular;//スペキュラ色
		XMFLOAT3 ambient;//アンビエント色
		unsigned char toonIdx;//トゥーン番号
		unsigned char edgeFlg;//マテリアルごとの輪郭線フラグ

		//２バイトのパディングあり

		uint32_t indicesNum;//このマテリアルが割り当てられる

		char texFilePath[20];//テクスチャファイルパス+α

	};//計７０バイトだが、２バイトのパディングがあるため７２バイトになる
#pragma pack()
	//ヘッダー
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

	//*ファイルオープン*----------------------------------
	HRESULT result;
	FILE* fp = nullptr;
	PMDHeader pmdheader = {};

	string modelPath = path;

	char signature[3] = {};//シグネチャ
	//string strModelPath = "Model/初音ミクmetal.pmd";
	result = fopen_s(&fp, modelPath.c_str(), "rb");//ファイルを開く
	if (FAILED(result)) { assert(0); }

	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	//*頂点関連*------------------------------------------
	uint32_t vertnum;//頂点数
	fread(&vertnum, sizeof(vertnum), 1, fp);
	constexpr unsigned int pmdvertex_size = sizeof(PMDVertex) - 2;//頂点当たりのサイズ
	std::vector<unsigned char> vertices(vertnum * pmdvertex_size);// 頂点データ配列
	vertices.resize(vertnum * pmdvertex_size);//バッファの確保
	fread(vertices.data(), vertices.size(), 1, fp);//読み込み

	//頂点バッファーの生成
	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	D3D12_RESOURCE_DESC resdesc = {};
	resdesc = CD3DX12_RESOURCE_DESC::Buffer(
		vertices.size() * sizeof(vertices[0]));

	//頂点バッファのリソース生成
	result = device->CreateCommittedResource(
		&heapprop, D3D12_HEAP_FLAG_NONE,
		&resdesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	//頂点情報のコピー
	unsigned char* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	vertBuff->Unmap(0, nullptr);//マップ解除

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertices.size();
	vbView.StrideInBytes = pmdvertex_size;

	//*頂点インデックス関連*------------------------------
	uint32_t indicesnum;
	fread(&indicesnum, sizeof(indicesnum), 1, fp);
	std::vector<unsigned short> indices(indicesnum);// 頂点インデックス配列	
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

	//*マテリアル関連*------------------------------------
//マテリアルの読み込み
	fread(&materialNum, sizeof(materialNum), 1, fp);

	materials.resize(materialNum);
	textureResources.resize(materialNum);
	sphResources.resize(materialNum);
	spaResources.resize(materialNum);

	std::vector<PMDMaterial>pmdMaterials(materialNum);
	fread(pmdMaterials.data(), pmdMaterials.size() * sizeof(PMDMaterial), 1, fp);
	//コピー
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

		//モデルとテクスチャパスからアプリケーションからのテクスチャパスを得る
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

	//*ファイルオープン*----------------------------------
	vmdMotion data;
	HRESULT result;
	FILE* fp = nullptr;
	string modelPath = path;

	result = fopen_s(&fp, modelPath.c_str(), "rb");//ファイルを開く
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

	//ソート
	for (auto& motion : data._motionData) {
		std::sort(
			motion.second.begin(),
			motion.second.end(),
			[](const Motion& lval, const Motion& rval) {
				return lval.frameNo <= rval.frameNo;
			});
	}

	recursiveMatrixMultiply(&_boneNodeTable["センター"], XMMatrixIdentity());
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

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

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
		&CD3DX12_RESOURCE_DESC::Buffer(materiaBuffSize * materialNum),//メモリがもったいない
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&materialBuff));
	if (FAILED(result)) { assert(0); }

	//マップマテリアルにコピー
	char* mapMaterial = nullptr;
	result = materialBuff->Map(0, nullptr, (void**)&mapMaterial);
	if (FAILED(result)) { assert(0); }

	for (auto& m : materials) {
		*((MaterialForHlsl*)mapMaterial) = m.material;//データコピー
		mapMaterial += materiaBuffSize;//次のアライメント位置まで
	}
	materialBuff->Unmap(0, nullptr);

	return S_OK;
}

HRESULT PMDmodel::CreateBone(FILE* fp)
{
	//ボーン数取得
	unsigned short boneNum = 0;
	fread(&boneNum, sizeof(boneNum), 1, fp);

	//ボーンデータ取得
	pmdBones.resize(boneNum);
	fread(pmdBones.data(), sizeof(PMDBone), boneNum, fp);

	vector<string> boneNames(pmdBones.size());

	//ボーンノードマップ作成
	for (int idx = 0; idx < pmdBones.size(); idx++)
	{
		auto& pb = pmdBones[idx];
		boneNames[idx] = pb.boneName;
		auto& node = _boneNodeTable[pb.boneName];
		node.boneIdx = idx;
		node.startPos = pb.pos;
	}

	//親子関係を構築
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

	//ボーンを初期化
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

	//通常テクスチャビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//RGBA(正規化)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;//
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;//ミップマップは使用しない

	//先頭を記録
	auto matDescHeapH = dx12->GetDescHeap()->GetCPUDescriptorHandleForHeapStart();
	auto inc = dx12->GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	matDescHeapH.ptr += inc;
	matDescHeapH.ptr += inc;
	for (int i = 0; i < materialNum; ++i) {
		//マテリアル用定数バッファビュー
		dx12->GetDevice()->CreateConstantBufferView(&matCBVDesc, matDescHeapH);
		matDescHeapH.ptr += inc;
		matCBVDesc.BufferLocation += materiaBuffSize;

		//シェーダーリソースビュー
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

		//乗算スフィアマップ用ビュー
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

		//加算スフィアマップ用ビュー
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
	//ディスクリプタヒープ生成(汎用)
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//どんなビューを作るのか()
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1024;//全体のヒープ領域数
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//特に指定なし

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
			motions.rbegin(),//リバースイテレーター
			motions.rend(),//リバースイテレーター
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
				XMQuaternionSlerp(rit->quaternion, it->quaternion, t));//球面線形補間
		}
		else {
			rotation = XMMatrixRotationQuaternion(rit->quaternion);
		}

		auto mat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z)
			* rotation
			* XMMatrixTranslation(pos.x, pos.y, pos.z);

		boneMatrices[node.boneIdx] = mat;
	}

	recursiveMatrixMultiply(&_boneNodeTable["センター"], XMMatrixIdentity());
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
