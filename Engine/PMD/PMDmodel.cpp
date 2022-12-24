#include "PMDmodel.h"
#include "pmdObject3D.h"

#include <d3dcompiler.h>
#include <DirectXTex.h>
#include<fstream>
#include<sstream>
#include <algorithm>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib,"winmm.lib")

#include "object\baseObject.h"
#include "dx12Wrapper.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

Wrapper* PMDmodel::dx12 = nullptr;
ComPtr<ID3D12DescriptorHeap> PMDmodel::materialDescHeap;

namespace {
	//モデルのパスとテクスチャのパスから合成パスを得る
	//@param modelpath アプリケーションから見たpmdモデルのパス
	//@param texPath PMD モデルから見たテクスチャのパス
	//@return アプリケーションから見たテクスチャのパス
	static string GetTexturePathFromModelAndTexPath(const string& modelPath, const char* texPath) {
		int pathIndex1 = (int)modelPath.rfind('/');
		int pathIndex2 = (int)modelPath.rfind('\\');

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
		size_t idx = path.rfind('.');
		return path.substr(idx + 1, path.length() - idx - 1);
	}

	//テクスチャのパスをセパレーター文字で取得する
	//@param path 対象のパス文字列
	//@param splitter 区切り文字
	//@return 分離前後の文字列ペア
	static std::pair<string, string>SplitFileName(const string& path, const char splitter = '*') {
		size_t idx = path.find(splitter);
		std::pair<string, string>ret;
		ret.first = path.substr(0, idx);
		ret.second = path.substr((idx + 1), path.length() - idx - 1);

		return ret;
	}

	XMMATRIX LookAtMatrix(const XMVECTOR& lookAt, XMFLOAT3& up, XMFLOAT3& right)
	{
		XMVECTOR vz = lookAt;

		XMVECTOR vy = XMVector3Normalize(XMLoadFloat3(&up));

		//XMVECTOR vx = XMVector3Normalize(XMVector3Cross(vz, vx));
		XMVECTOR vx = XMVector3Normalize(XMVector3Cross(vy, vz));
		vy = XMVector3Normalize(XMVector3Cross(vz, vx));

		if (std::abs(XMVector3Dot(vy, vz).m128_f32[0]) == 1.0f) {
			vx = XMVector3Normalize(XMLoadFloat3(&right));

			vy = XMVector3Normalize(XMVector3Cross(vz, vx));

			vx = XMVector3Normalize(XMVector3Cross(vy, vz));
		}

		XMMATRIX ret = XMMatrixIdentity();
		ret.r[0] = vx;
		ret.r[1] = vy;
		ret.r[2] = vz;

		return ret;
	}
	XMMATRIX LookAtMatrix(const XMVECTOR& origin, const XMVECTOR& lookAt, XMFLOAT3& up, XMFLOAT3& right)
	{
		return XMMatrixTranspose(LookAtMatrix(origin, up, right)) * LookAtMatrix(lookAt, up, right);
	}

	enum class BoneType {
		Rotation,
		RotAndMove,
		IK,
		Underfined,
		IKChild,
		RotationChild,
		IKDestination,
		Invisible
	};
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
	texresDesc.Height = UINT(metadata.height);
	texresDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	texresDesc.SampleDesc.Count = 1;
	texresDesc.SampleDesc.Quality = 0;
	texresDesc.MipLevels = UINT16(metadata.mipLevels);
	texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	texresDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;//
	texresDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//テクスチャバッファのリソース生成
	result = dx12->GetDevice()->CreateCommittedResource(
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
		UINT(img->rowPitch),
		UINT(img->slicePitch));
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
	auto result = dx12->GetDevice()->CreateCommittedResource(
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
		(UINT)data.size());

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
	auto result = dx12->GetDevice()->CreateCommittedResource(
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
		(UINT)data.size());

	return blackBuff;
}

void* PMDmodel::Transform::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

PMDmodel::PMDmodel(/*Wrapper* _dx12, const char* filepath, PMDobject& object*/)
//_object(object)
{
	//// 再初期化チェック
	//assert(!PMDmodel::dx12);

	//// nullptrチェック
	//assert(_dx12->GetDevice());

	//PMDmodel::dx12 = _dx12;

	//transform.world = XMMatrixIdentity();
	//LoadPMDFile(filepath);
	//CreateDescHeap();
	//CreateTransform();
	//LoadVMDFile(vmdData::WALK, "Resources/vmd/Rick式走りモーション02.vmd");
	//LoadVMDFile(vmdData::WAIT, "Resources/vmd/marieru_stand.vmd");
	//LoadVMDFile(vmdData::ATTACK, "Resources/vmd/test.vmd");
	//CreateMaterial();
	//CreateMaterialAndTextureView();
}

PMDmodel::~PMDmodel()
{
}

bool PMDmodel::StaticInitialize(Wrapper* _dx12)
{
	// 再初期化チェック
	assert(!PMDmodel::dx12);

	// nullptrチェック
	assert(_dx12->GetDevice());

	//設定
	PMDmodel::dx12 = _dx12;

	return false;
}

PMDmodel* PMDmodel::CreateFromPMD(const char* filepath)
{
	PMDmodel* instance = new PMDmodel();
	if (instance == nullptr) {
		return nullptr;
	}

	if (!instance->Initialize(filepath)) {
		delete instance;
		assert(0);
	}

	return instance;
}

bool PMDmodel::Initialize(const char* filepath)
{
	LoadPMDFile(filepath);

	if (CreateDescHeap() != S_OK)assert(0);
	if (CreateTransform() != S_OK)assert(0);
	if (CreateMaterial() != S_OK)assert(0);
	if (CreateMaterialAndTextureView() != S_OK)assert(0);

	return true;
}
void PMDmodel::Update()
{
	HRESULT result;
	//// スケール、回転、平行移動行列の計算
	//XMMATRIX matScale, matTrans;
	//matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//// ワールド行列の合成
	//matWorld = XMMatrixIdentity(); // 変形をリセット
	//matWorld *= matScale; // ワールド行列にスケーリングを反映
	//matWorld *= matRot; // ワールド行列に回転を反映
	//matWorld *= matTrans; // ワールド行列に平行移動を反映

	UpdateWorldMatrix();

	result = transformBuff->Map(0, nullptr, (void**)&_mappedMatrices);
	if (FAILED(result)) { assert(0); }
	_mappedMatrices[0] = matWorld;
	copy(boneMatrices.begin(), boneMatrices.end(), _mappedMatrices + 1);

	transformBuff->Unmap(0, nullptr);

	MotionUpdate();
}
void PMDmodel::UpdateWorldMatrix()
{
	// スケール、回転、平行移動行列の計算
	XMMATRIX matScale, matTrans;
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映
}
void PMDmodel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptrチェック
	assert(dx12->GetDevice());
	assert(cmdList);

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
		cmdList->DrawIndexedInstanced(m.indicesNum, 2, idxOffset, 0, 0);

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

#pragma pack(1)//苦肉の策
	struct PMDVertexFile {
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
		unsigned short boneNo[2];
		unsigned char boneWeight;
		unsigned char edgeFlg;
	};
#pragma pack()
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
	if (fp == 0) { assert(0); }

	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	//*頂点関連*------------------------------------------
	uint32_t vertnum;//頂点数
	fread(&vertnum, sizeof(vertnum), 1, fp);
	constexpr unsigned int pmdvertex_size = sizeof(PMDVertex);//頂点当たりのサイズ
	constexpr unsigned int pmdvertexFile_size = sizeof(PMDVertexFile);//頂点当たりのサイズ
	std::vector<unsigned char> vertices(vertnum * pmdvertexFile_size);// 頂点データ配列
	vertices.resize(vertnum * pmdvertexFile_size);//バッファの確保
	fread(vertices.data(), vertices.size(), 1, fp);//読み込み

	std::vector<PMDVertex> t_vertices;

	for (uint32_t i = 0; i < vertnum; ++i)
	{
		PMDVertexFile p =
			reinterpret_cast<PMDVertexFile*>(vertices.data())[i];
		PMDVertex vert;
		vert.pos = p.pos;
		vert.normal = p.normal;
		vert.uv = p.uv;
		vert.boneNo[0] = p.boneNo[0];
		vert.boneNo[1] = p.boneNo[1];
		vert.boneWeight = p.boneWeight;
		vert.edgeFlg = p.edgeFlg;

		t_vertices.push_back(vert);
	}

	//頂点バッファーの生成
	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

#if 0
	D3D12_RESOURCE_DESC resdesc = {};
	resdesc = CD3DX12_RESOURCE_DESC::Buffer(
		vertices.size() /** sizeof(vertices[0])*/);
#endif
	D3D12_RESOURCE_DESC resdesc = {};
	resdesc = CD3DX12_RESOURCE_DESC::Buffer(
		t_vertices.size() * sizeof(t_vertices[0]));


	//頂点バッファのリソース生成
	result = dx12->GetDevice()->CreateCommittedResource(
		&heapprop, D3D12_HEAP_FLAG_NONE,
		&resdesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	//頂点情報のコピー
	unsigned char* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::memcpy(vertMap, &t_vertices[0], t_vertices.size() * sizeof(t_vertices[0]));
	//std::copy(t_vertices.begin(), t_vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);//マップ解除

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = (UINT)t_vertices.size() * sizeof(t_vertices[0]);
	vbView.StrideInBytes = sizeof(t_vertices[0]);

	//D3D12_RESOURCE_DESC resdesc = {};
	//resdesc = CD3DX12_RESOURCE_DESC::Buffer(
	//	t_vertices.size() * sizeof(t_vertices[0]));

	////頂点バッファのリソース生成
	//result = device->CreateCommittedResource(
	//	&heapprop, D3D12_HEAP_FLAG_NONE,
	//	&resdesc, D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr, IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf()));
	//if (FAILED(result)) { assert(0); }

	////頂点情報のコピー
	//PMDVertex* vertMap = nullptr;
	//result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	//std::copy(std::begin(t_vertices), std::end(t_vertices), vertMap);
	//vertBuff->Unmap(0, nullptr);//マップ解除

	//vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//vbView.SizeInBytes = t_vertices.size();
	//vbView.StrideInBytes = pmdvertex_size;

	//*頂点インデックス関連*------------------------------
	uint32_t indicesnum;
	fread(&indicesnum, sizeof(indicesnum), 1, fp);
	std::vector<unsigned short> indices(indicesnum);// 頂点インデックス配列	
	fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);

	resdesc = CD3DX12_RESOURCE_DESC::Buffer(
		indices.size() * sizeof(indices[0]));

	result = dx12->GetDevice()->CreateCommittedResource(
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
	ibView.SizeInBytes = (UINT)indices.size() * sizeof(indices[0]);

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
		string spaFileName = "";
		if (count(texFileName.begin(), texFileName.end(), '*') > 0)
		{
			auto namepair = SplitFileName(texFileName);
			if (GetExtension(namepair.first) == "sph")
			{
				texFileName = namepair.second;
				sphFileName = namepair.first;
			}
			else if (GetExtension(namepair.first) == "spa") {
				texFileName = namepair.second;
				spaFileName = namepair.first;
			}
			else {
				texFileName = namepair.first;
				if (GetExtension(namepair.second) == "sph") {
					sphFileName = namepair.second;
				}
				else if (GetExtension(namepair.second) == "spa") {
					spaFileName = namepair.second;
				}
			}
		}
		else {
			if (GetExtension(pmdMaterials[i].texFilePath) == "sph") {
				sphFileName = pmdMaterials[i].texFilePath;
				texFileName = "";
			}
			else if (GetExtension(pmdMaterials[i].texFilePath) == "spa") {
				spaFileName = pmdMaterials[i].texFilePath;
				texFileName = "";
			}
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
		if (spaFileName != "") {
			auto spaFilePath = GetTexturePathFromModelAndTexPath(modelPath, spaFileName.c_str());
			spaResources[i] = LoadTextureFromFile(spaFilePath);
		}
	}

	CreateBone(fp);

	uint16_t ikNum = 0;
	fread(&ikNum, sizeof(ikNum), 1, fp);

	std::vector<PMDIK> pmdIkData(ikNum);
	_ikData.resize(ikNum);
	for (auto& ik : _ikData) {
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
		XMFLOAT4 quaternion;
		unsigned char bezier[64];
	};

	//*ファイルオープン*----------------------------------
	vmdMotion data;
	HRESULT result;
	FILE* fp = nullptr;
	string modelPath = path;

	result = fopen_s(&fp, modelPath.c_str(), "rb");//ファイルを開く
	if (FAILED(result)) { assert(0); }
	if (fp == 0) { assert(0); }

	fseek(fp, 50, SEEK_SET);
	unsigned int motionDataNum = 0;
	fread(&motionDataNum, sizeof(motionDataNum), 1, fp);

	vector<VMDMotion> vmdMotionData(motionDataNum);

	for (auto& motion : vmdMotionData)
	{
		fread(motion.boneName, sizeof(motion.boneName), 1, fp);
		fread(&motion.frameNo, sizeof(motion.frameNo) +
			sizeof(motion.location) +
			sizeof(motion.quaternion) +
			sizeof(motion.bezier), 1, fp);
	}

#pragma pack(1)
	//表情データ(頂点モーフデータ)
	struct VMDMorph {
		char name[15];//名前(パディングしてしまう)
		uint32_t frameNo;//フレーム番号
		float weight;//ウェイト(0.0f～1.0f)
	};//全部で23バイトなのでpragmapackで読む
#pragma pack()

	//表情データ読み込み
	uint32_t morphCount = 0;
	fread(&morphCount, sizeof(morphCount), 1, fp);
	vector<VMDMorph> morphs(morphCount);
	fread(morphs.data(), sizeof(VMDMorph), morphCount, fp);

#pragma pack(1)
	//カメラ
	struct VMDCamera {
		uint32_t frameNo; // フレーム番号
		float distance; // 距離
		XMFLOAT3 pos; // 座標
		XMFLOAT3 eulerAngle; // オイラー角
		uint8_t Interpolation[24]; // 補完
		uint32_t fov; // 視界角
		uint8_t persFlg; // パースフラグON/OFF
	};//61バイト(これもpragma pack(1)の必要あり)
#pragma pack()

	//読み込み
	uint32_t vmdCameraCount = 0;
	fread(&vmdCameraCount, sizeof(vmdCameraCount), 1, fp);
	vector<VMDCamera> cameraData(vmdCameraCount);
	fread(cameraData.data(), sizeof(VMDCamera), vmdCameraCount, fp);

	// ライト照明データ
	struct VMDLight {
		uint32_t frameNo; // フレーム番号
		XMFLOAT3 rgb; //ライト色
		XMFLOAT3 vec; //光線ベクトル(平行光線)
	};

	//読み込み
	uint32_t vmdLightCount = 0;
	fread(&vmdLightCount, sizeof(vmdLightCount), 1, fp);
	vector<VMDLight> lights(vmdLightCount);
	fread(lights.data(), sizeof(VMDLight), vmdLightCount, fp);

#pragma pack(1)
	// セルフ影データ
	struct VMDSelfShadow {
		uint32_t frameNo; // フレーム番号
		uint8_t mode; //影モード(0:影なし、1:モード１、2:モード２)
		float distance; //距離
	};
#pragma pack()

	//読み込み
	uint32_t selfShadowCount = 0;
	fread(&selfShadowCount, sizeof(selfShadowCount), 1, fp);
	vector<VMDSelfShadow> selfShadowData(selfShadowCount);
	fread(selfShadowData.data(), sizeof(VMDSelfShadow), selfShadowCount, fp);

	//IKオン/オフの切り替わり数
	uint32_t ikSwicthCount = 0;
	fread(&ikSwicthCount, sizeof(ikSwicthCount), 1, fp);

	_ikEnableData.resize(ikSwicthCount);
	for (auto& ikEnable : _ikEnableData)
	{
		//キーフレーム番号読み込み
		fread(&ikEnable.freamNo, sizeof(ikEnable.freamNo), 1, fp);
		//可視化フラグ読み込み
		uint8_t visibleFlg = 0;
		fread(&visibleFlg, sizeof(visibleFlg), 1, fp);
		//対象ボーン数読み込み
		uint32_t ikBoneCount = 0;
		fread(&ikBoneCount, sizeof(ikBoneCount), 1, fp);

		//名前とオン/オフ情報取得
		for (uint32_t i = 0; i < ikBoneCount; ++i)
		{
			char ikBoneName[20];
			fread(ikBoneName, _countof(ikBoneName), 1, fp);

			uint8_t flg = 0;
			fread(&flg, sizeof(flg), 1, fp);
			ikEnable.ikEnableTable[ikBoneName] = flg;
		}
	}
	fclose(fp);

	for (auto& vmdMotion : vmdMotionData) {
		data._motionData[vmdMotion.boneName].emplace_back(
			Motion(
				vmdMotion.frameNo,
				DirectX::XMLoadFloat4(&vmdMotion.quaternion),
				vmdMotion.location,
				XMFLOAT2((float)vmdMotion.bezier[3] / 127.0f, (float)vmdMotion.bezier[7] / 127.0f),
				XMFLOAT2((float)vmdMotion.bezier[11] / 127.0f, (float)vmdMotion.bezier[15] / 127.0f)
			));

		data.duration = std::max<unsigned int>(data.duration, vmdMotion.frameNo);
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

	for (auto& boneMoiton : data._motionData) {

		auto itBoneNode = _boneNodeTable.find(boneMoiton.first);
		if (itBoneNode == _boneNodeTable.end()) {
			continue;
		}

		auto node = itBoneNode->second;
		auto& pos = node.startPos;
		auto mat =
			XMMatrixTranslation(-pos.x, -pos.y, -pos.z)
			* XMMatrixRotationQuaternion(boneMoiton.second[0].quaternion)
			* XMMatrixTranslation(pos.x, pos.y, pos.z);
		boneMatrices[node.boneIdx] = mat;
	}

	data.invBoneMatrices.resize(pmdBones.size());
	//ボーンを初期化
	std::fill(data.invBoneMatrices.begin(), data.invBoneMatrices.end(), XMMatrixIdentity());

	for (int i = 0; i < boneMatrices.size(); i++) {
		data.invBoneMatrices[i] = XMMatrixInverse(nullptr, boneMatrices[i]);
	}

	recursiveMatrixMultiply(&_boneNodeTable["センター"], XMMatrixIdentity());
	copy(boneMatrices.begin(), boneMatrices.end(), _mappedMatrices + 1);

	std::fill(
		boneMatrices.begin(),
		boneMatrices.end(),
		XMMatrixIdentity());

	motion.insert(std::make_pair(Number, data));

	return S_OK;
}

HRESULT PMDmodel::CreateTransform()
{
	auto buffSize = sizeof(XMMATRIX) * (1 + boneMatrices.size());
	buffSize = (buffSize + 0xff) & ~0xff;

	CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	auto result = dx12->GetDevice()->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
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
	cbvDesc.SizeInBytes = (UINT)buffSize;
	dx12->GetDevice()->CreateConstantBufferView(
		&cbvDesc,
		heapHandle);

	return S_OK;
}

HRESULT PMDmodel::CreateMaterial()
{
	auto materiaBuffSize = sizeof(MaterialForHlsl);
	materiaBuffSize = (materiaBuffSize + 0xff) & ~0xff;

	CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(materiaBuffSize * materialNum);
	auto result = dx12->GetDevice()->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,//メモリがもったいない
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

	_boneNameArray.resize(pmdBones.size());
	_boneNodeAddressArray.resize(pmdBones.size());

	//ボーンノードマップ作成
	_kneeIdxes.clear();
	for (int idx = 0; idx < pmdBones.size(); idx++)
	{
		auto& pb = pmdBones[idx];
		boneNames[idx] = pb.boneName;
		auto& node = _boneNodeTable[pb.boneName];
		node.boneIdx = idx;
		node.startPos = pb.pos;

		_boneNameArray[idx] = pb.boneName;
		_boneNodeAddressArray[idx] = &node;

		std::string boneName = pb.boneName;
		if (boneName.find("ひざ") != std::string::npos) {
			_kneeIdxes.emplace_back(idx);
		}
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
	//invBoneMatrices.resize(pmdBones.size());

	//ボーンを初期化
	//std::fill(
	//	boneMatrices.begin(),
	//	boneMatrices.end(),
	//	XMMatrixIdentity());

	//for (int i = 0; i < boneMatrices.size(); i++) {
	//	invBoneMatrices[i] = XMMatrixInverse(nullptr, boneMatrices[i]);
	//}

	auto getNameFromIdx = [&](uint16_t idx)->string {
		auto it = find_if(_boneNodeTable.begin(), _boneNodeTable.end(), [idx](const pair<string, BoneNode>& obj) {
			return obj.second.boneIdx == idx;
			});
		if (it != _boneNodeTable.end()) {
			return it->first;
		}
		else {
			return "";
		}
	};
	for (auto& ik : _ikData)
	{
		std::ostringstream oss;
		oss << "IKボーン番号=" << ik.boneIdx << ":" << getNameFromIdx(ik.boneIdx) << endl;
		for (auto& node : ik.nodeIdx) {
			oss << "\tノードボーン=" << node << ":" << getNameFromIdx(node) << endl;
		}
		OutputDebugStringA(oss.str().c_str());
	}
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
	matCBVDesc.SizeInBytes = (UINT)materiaBuffSize;

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
	for (uint32_t i = 0; i < materialNum; ++i) {
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
	//攻撃中は変更を受け付けない
	if (oldVmdNumber == vmdData::ATTACK) {
		oldVmdNumber = vmdData::ATTACK;
		vmdNumber = vmdData::ATTACK;
		data = motion.at(vmdData::ATTACK);
	}

	if (oldVmdNumber == vmdData::DAMAGE) {
		oldVmdNumber = vmdData::DAMAGE;
		vmdNumber = vmdData::DAMAGE;
		data = motion.at(vmdData::DAMAGE);
	}

	unsigned long elapsedTime = (unsigned long)(timeGetTime() - _startTime);

	unsigned int frameNo = (int)(30 * (elapsedTime / 1000.0f));

	if (vmdNumber != oldVmdNumber) {
		_startTime = timeGetTime();
		frameNo = 0;
	}

	if (frameNo > data.duration) {
		if (oldVmdNumber == vmdData::ATTACK) {
			oldVmdNumber = vmdData::WAIT;
			vmdNumber = vmdData::WAIT;
		}
		if (oldVmdNumber == vmdData::DAMAGE) {
			oldVmdNumber = vmdData::WAIT;
			vmdNumber = vmdData::WAIT;
		}

		_startTime = timeGetTime();
		frameNo = 0;
	}

	std::fill(boneMatrices.begin(), boneMatrices.end(), XMMatrixIdentity());

	for (auto& boneMoiton : data._motionData) {
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

		//auto& pos = position;

		XMMATRIX rotation;
		XMVECTOR offset = XMLoadFloat3(&rit->offset);
		auto it = rit.base();

		if (it != motions.end()) {
			auto t = static_cast<float>(frameNo - rit->frameNo)
				/ static_cast<float>(it->frameNo - rit->frameNo);

			t = GetYFromXOn(t, it->p1, it->p2, 9);

			rotation = XMMatrixRotationQuaternion(
				XMQuaternionSlerp(rit->quaternion, it->quaternion, t));//球面線形補間

			offset = XMVectorLerp(offset, XMLoadFloat3(&it->offset), t);
		}
		else {
			rotation = XMMatrixRotationQuaternion(rit->quaternion);
		}

		auto& itBoneName = boneMoiton.first;
		auto itBoneNode = _boneNodeTable.find(itBoneName);
		if (itBoneNode == _boneNodeTable.end()) { continue; }
		auto node = itBoneNode->second;
		auto& pos = node.startPos;

		rotation =
			XMMatrixTranslation(-pos.x, -pos.y, -pos.z) * //原点に移動して
			rotation *								      //回転させて
			XMMatrixTranslation(pos.x, pos.y, pos.z);     //元の位置に戻す

		boneMatrices[node.boneIdx] =
			//data.invBoneMatrices[node.boneIdx] *
			rotation *
			XMMatrixTranslationFromVector(offset);
	}

	recursiveMatrixMultiply(&_boneNodeTable["センター"], XMMatrixIdentity());

	if (a == true) {
		IKSolve(frameNo);
	}

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

void PMDmodel::IKSolve(uint32_t frameNo)
{
	//IKオン/オフ情報をフレーム番号で逆から検索
	auto it = find_if(_ikEnableData.rbegin(), _ikEnableData.rend(),
		[frameNo](const VMDIKEnable& ikenable) {
			return ikenable.freamNo <= frameNo;
		});

	//IK解決用ループ
	for (auto& ik : _ikData)
	{
		if (it != _ikEnableData.rend()) {
			auto ikEnableIt =
				it->ikEnableTable.find(_boneNameArray[ik.boneIdx]);

			if (ikEnableIt != it->ikEnableTable.end()) {
				if (!ikEnableIt->second) {
					continue;
				}
			}
		}

		auto childrenNodesCount = ik.nodeIdx.size();

		switch (childrenNodesCount)
		{
		case 0:
			assert(0);
			continue;
		case 1:
			SolveLookAt(ik);
			break;
		case 2:
			SolveConsineIK(ik);
			break;
		default:
			SolveCCOIK(ik);
			break;
		}
	}
}

constexpr float epsilon = 0.0005f;
void PMDmodel::SolveConsineIK(const PMDIK& ik)
{
	vector<XMVECTOR> positions;//IK構成点を保存
	std::array<float, 2> edgeLens;//IKのそれぞれのボーン間の距離を保存

	//ターゲット
	auto& targetNode = _boneNodeAddressArray[ik.boneIdx];
	auto targetPos = XMVector3Transform(
		XMLoadFloat3(&targetNode->startPos),
		boneMatrices[ik.boneIdx]);

	//末端ボーン
	auto endNode = _boneNodeAddressArray[ik.targetidx];
	positions.emplace_back(XMLoadFloat3(&endNode->startPos));

	//中間及びルートボーン
	for (auto& chainBoneIdx : ik.nodeIdx)
	{
		auto boneNode = _boneNodeAddressArray[chainBoneIdx];
		positions.emplace_back(XMLoadFloat3(&boneNode->startPos));
	}

	reverse(positions.begin(), positions.end());

	//元の長さ
	edgeLens[0] = XMVector3Length(XMVectorSubtract(positions[1], positions[0])).m128_f32[0];
	edgeLens[1] = XMVector3Length(XMVectorSubtract(positions[2], positions[1])).m128_f32[0];

	//ルートボーン座標変換
	positions[0] = XMVector3Transform(positions[0], boneMatrices[ik.nodeIdx[1]]);

	//真ん中はスルー

	//先端ボーン
	positions[2] = XMVector3Transform(positions[2], boneMatrices[ik.boneIdx]);

	//ルートから先端へのベクトルを作る
	auto linerVec = XMVectorSubtract(positions[2], positions[0]);

	float A = XMVector3Length(linerVec).m128_f32[0];
	float B = edgeLens[0];
	float C = edgeLens[1];

	linerVec = XMVector3Normalize(linerVec);

	float theta1 = acosf((A * A + B * B - C * C) / (2 * A * B));

	float theta2 = acosf((B * B + C * C - A * A) / (2 * B * C));

	XMVECTOR axis;
	if (find(_kneeIdxes.begin(), _kneeIdxes.end(), ik.nodeIdx[0]) == _kneeIdxes.end()) {
		auto vm = XMVector3Normalize(XMVectorSubtract(positions[2], positions[0]));
		auto vt = XMVector3Normalize(XMVectorSubtract(targetPos, positions[0]));
		axis = XMVector3Cross(vt, vm);
	}
	else {
		auto right = XMFLOAT3(1, 0, 0);
		axis = XMLoadFloat3(&right);
	}

	auto mat1 = XMMatrixTranslationFromVector(-positions[0]);
	mat1 *= XMMatrixRotationAxis(axis, theta1);
	mat1 *= XMMatrixTranslationFromVector(positions[0]);

	auto mat2 = XMMatrixTranslationFromVector(-positions[1]);
	mat2 *= XMMatrixRotationAxis(axis, theta2 - XM_PI);
	mat2 *= XMMatrixTranslationFromVector(positions[1]);

	boneMatrices[ik.nodeIdx[1]] *= mat1;
	boneMatrices[ik.nodeIdx[0]] = mat2 * boneMatrices[ik.nodeIdx[1]];
	boneMatrices[ik.targetidx] = boneMatrices[ik.nodeIdx[0]];

	//vector<XMVECTOR> positions;//IK構成点を保存
	//std::array<float, 2> edgeLens;//IKのそれぞれのボーン間の距離を保存

	////ターゲット(末端ボーンではなく、末端ボーンが近づく目標ボーンの座標を取得)
	//auto& targetNode = _boneNodeAddressArray[ik.boneIdx];
	//auto targetPos = XMVector3Transform(XMLoadFloat3(&targetNode->startPos), boneMatrices[ik.boneIdx]);

	////IKチェーンが逆順なので、逆に並ぶようにしている
	////末端ボーン
	//auto endNode = _boneNodeAddressArray[ik.targetidx];
	//positions.emplace_back(XMLoadFloat3(&endNode->startPos));
	////中間及びルートボーン
	//for (auto& chainBoneIdx : ik.nodeIdx) {
	//	auto boneNode = _boneNodeAddressArray[chainBoneIdx];
	//	positions.emplace_back(XMLoadFloat3(&boneNode->startPos));
	//}
	////ちょっと分かりづらいと思ったので逆にしておきます。そうでもない人はそのまま
	////計算してもらって構わないです。
	//reverse(positions.begin(), positions.end());

	////元の長さを測っておく
	//edgeLens[0] = XMVector3Length(XMVectorSubtract(positions[1], positions[0])).m128_f32[0];
	//edgeLens[1] = XMVector3Length(XMVectorSubtract(positions[2], positions[1])).m128_f32[0];

	////ルートボーン座標変換(逆順になっているため使用するインデックスに注意)
	//positions[0] = XMVector3Transform(positions[0], boneMatrices[ik.nodeIdx[1]]);
	////真ん中はどうせ自動計算されるので計算しない
	////先端ボーン
	//positions[2] = XMVector3Transform(positions[2], boneMatrices[ik.boneIdx]);//ホンマはik.targetIdxだが…！？

	////ルートから先端へのベクトルを作っておく
	//auto linearVec = XMVectorSubtract(positions[2], positions[0]);
	//float A = XMVector3Length(linearVec).m128_f32[0];
	//float B = edgeLens[0];
	//float C = edgeLens[1];

	//linearVec = XMVector3Normalize(linearVec);

	////ルートから真ん中への角度計算
	//float theta1 = acosf((A * A + B * B - C * C) / (2 * A * B));

	////真ん中からターゲットへの角度計算
	//float theta2 = acosf((B * B + C * C - A * A) / (2 * B * C));

	////「軸」を求める
	////もし真ん中が「ひざ」であった場合には強制的にX軸とする。
	//XMVECTOR axis;
	//if (find(_kneeIdxes.begin(), _kneeIdxes.end(), ik.nodeIdx[0]) == _kneeIdxes.end()) {
	//	auto vm = XMVector3Normalize(XMVectorSubtract(positions[2], positions[0]));
	//	auto vt = XMVector3Normalize(XMVectorSubtract(targetPos, positions[0]));
	//	axis = XMVector3Cross(vt, vm);
	//}
	//else {
	//	auto right = XMFLOAT3(1, 0, 0);
	//	axis = XMLoadFloat3(&right);
	//}

	////注意点…IKチェーンは根っこに向かってから数えられるため1が根っこに近い
	//auto mat1 = XMMatrixTranslationFromVector(-positions[0]);
	//mat1 *= XMMatrixRotationAxis(axis, theta1);
	//mat1 *= XMMatrixTranslationFromVector(positions[0]);


	//auto mat2 = XMMatrixTranslationFromVector(-positions[1]);
	//mat2 *= XMMatrixRotationAxis(axis, theta2 - XM_PI);
	//mat2 *= XMMatrixTranslationFromVector(positions[1]);

	//boneMatrices[ik.nodeIdx[1]] *= mat1;
	//boneMatrices[ik.nodeIdx[0]] = mat2 * boneMatrices[ik.nodeIdx[1]];
	//boneMatrices[ik.targetidx] = boneMatrices[ik.nodeIdx[0]];//直前の影響を受ける

}
void PMDmodel::SolveLookAt(const PMDIK& ik)
{
	auto rootNode = _boneNodeAddressArray[ik.nodeIdx[0]];
	auto targetNode = _boneNodeAddressArray[ik.targetidx];

	auto rpos1 = XMLoadFloat3(&rootNode->startPos);
	auto tpos1 = XMLoadFloat3(&targetNode->startPos);

	auto rpos2 = XMVector3TransformCoord(rpos1, boneMatrices[ik.nodeIdx[0]]);
	auto tpos2 = XMVector3TransformCoord(tpos1, boneMatrices[ik.boneIdx]);

	auto originVec = XMVectorSubtract(tpos1, rpos1);
	auto targetVec = XMVectorSubtract(tpos2, rpos2);

	originVec = XMVector3Normalize(originVec);
	targetVec = XMVector3Normalize(targetVec);

	XMMATRIX mat = XMMatrixTranslationFromVector(-rpos2) *
		LookAtMatrix(originVec, targetVec, XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0)) *
		XMMatrixTranslationFromVector(rpos2);
	boneMatrices[ik.nodeIdx[0]] = mat;

	//boneMatrices[ik.nodeIdx[0]] = LookAtMatrix(originVec, targetVec, XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0));

	//	//この関数に来た時点でノードはひとつしかなく、チェーンに入っているノード番号は
	////IKのルートノードのものなので、このルートノードからターゲットに向かうベクトルを考えればよい
	//auto rootNode = _boneNodeAddressArray[ik.nodeIdx[0]];
	//auto targetNode = _boneNodeAddressArray[ik.targetidx];//!?

	//auto opos1 = XMLoadFloat3(&rootNode->startPos);
	//auto tpos1 = XMLoadFloat3(&targetNode->startPos);

	//auto opos2 = XMVector3Transform(opos1, boneMatrices[ik.nodeIdx[0]]);
	//auto tpos2 = XMVector3Transform(tpos1, boneMatrices[ik.boneIdx]);

	//auto originVec = XMVectorSubtract(tpos1, opos1);
	//auto targetVec = XMVectorSubtract(tpos2, opos2);

	//originVec = XMVector3Normalize(originVec);
	//targetVec = XMVector3Normalize(targetVec);

	//XMMATRIX mat = XMMatrixTranslationFromVector(-opos2) *
	//	LookAtMatrix(originVec, targetVec, XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0)) *
	//	XMMatrixTranslationFromVector(opos2);

	////auto parent = _boneNodeAddressArray[ik.boneIdx]->parentBone;

	//boneMatrices[ik.nodeIdx[0]] = mat;// _boneMatrices[ik.boneIdx] * _boneMatrices[parent];
	////_boneMatrices[ik.targetIdx] = _boneMatrices[parent];

}
void PMDmodel::SolveCCOIK(const PMDIK& ik)
{
	vector<XMVECTOR> bonePositions;//IK構成点を保存

	//ターゲットの座標を変換
	auto targeBoneNode = _boneNodeAddressArray[ik.boneIdx];
	auto targetOriginPos = XMLoadFloat3(&targeBoneNode->startPos);

	//親の行列変換を逆行列で無効化
	auto parentMat = boneMatrices[_boneNodeAddressArray[ik.boneIdx]->ikParentBone];
	XMVECTOR det;
	auto invParentMat = XMMatrixInverse(&det, parentMat);
	auto targetNextPos = XMVector3Transform(targetOriginPos, boneMatrices[ik.boneIdx] * invParentMat);

	//末端ノードの座標保存
	auto endPos = XMLoadFloat3(&_boneNodeAddressArray[ik.targetidx]->startPos);

	//中間ノード
	for (auto& cidx : ik.nodeIdx)
	{
		bonePositions.push_back(XMLoadFloat3(&_boneNodeAddressArray[cidx]->startPos));
	}

	//末端ボーン以外のボーンの数だけ行列を確保
	std::vector<XMMATRIX> mats(bonePositions.size());
	fill(mats.begin(), mats.end(), XMMatrixIdentity());

	//回転制限に乗算
	auto iklimit = ik.limit * XM_PI;

	//ikに設定されている試行回数だけ繰り返す
	for (int c = 0; c < ik.iterations; ++c)
	{
		//ターゲットと末端ボーンがほぼ一致していたら抜ける
		if (XMVector3Length(XMVectorSubtract(endPos, targetNextPos)).m128_f32[0] <= epsilon) {
			break;
		}

		//ボーンをさかのぼりながら
		//角度制限にひっかからないように
		for (int bidx = 0; bidx < bonePositions.size(); ++bidx)
		{
			const auto& pos = bonePositions[bidx];
			//対象ノードから末端ノードまでと対象ノードからターゲットまでのベクトル作成
			auto vecToEnd = XMVectorSubtract(endPos, pos);
			auto vecToTarget = XMVectorSubtract(targetNextPos, pos);

			//正規化
			vecToEnd = XMVector3Normalize(vecToEnd);
			vecToTarget = XMVector3Normalize(vecToTarget);

			//同じベクトルになった場合は次のボーンに引き渡す
			if (XMVector3Length(XMVectorSubtract(vecToEnd, vecToTarget)).m128_f32[0] < epsilon) {
				continue;
			}

			//外積計算と角度計算
			auto cross = XMVector3Normalize(XMVector3Cross(vecToEnd, vecToTarget));

			//便利な
			float angle = XMVector3AngleBetweenVectors(vecToEnd, vecToTarget).m128_f32[0];

			//回転限界超えたら補正
			angle = min(angle, iklimit);
			auto rot = XMMatrixRotationAxis(cross, angle);

			//原点中心ではなく
			auto mat = 
				XMMatrixTranslationFromVector(-pos) 
				* rot 
				* XMMatrixTranslationFromVector(pos);

			//回転行列
			mats[bidx] *= mat;

			for (auto idx = bidx - 1; idx >= 0; --idx) {
				bonePositions[idx] = XMVector3Transform(bonePositions[idx], mat);
			}

			endPos = XMVector3Transform(endPos, mat);

			//もし正確に近くになっていたら
			if (XMVector3Length(XMVectorSubtract(endPos, targetNextPos)).m128_f32[0] <= epsilon) {
				break;
			}
		}
	}

	int idx = 0;
	for (auto& cidx : ik.nodeIdx) {
		boneMatrices[cidx] = mats[idx];
		++idx;
	}

	auto rootNode = _boneNodeAddressArray[ik.nodeIdx.back()];
	recursiveMatrixMultiply(rootNode, parentMat);


	//	//ターゲット
	//auto targetBoneNode = _boneNodeAddressArray[ik.boneIdx];
	//auto targetOriginPos = XMLoadFloat3(&targetBoneNode->startPos);

	//auto parentMat = boneMatrices[_boneNodeAddressArray[ik.boneIdx]->ikParentBone];
	//XMVECTOR det;
	//auto invParentMat = XMMatrixInverse(&det, parentMat);
	//auto targetNextPos = XMVector3Transform(targetOriginPos, boneMatrices[ik.boneIdx] * invParentMat);


	////まずはIKの間にあるボーンの座標を入れておく(逆順注意)
	//std::vector<XMVECTOR> bonePositions;
	////auto endPos = XMVector3Transform(
	////	XMLoadFloat3(&_boneNodeAddressArray[ik.targetIdx]->startPos),
	////	//_boneMatrices[ik.targetIdx]);
	////	XMMatrixIdentity());
	////末端ノード
	//auto endPos = XMLoadFloat3(&_boneNodeAddressArray[ik.targetidx]->startPos);
	////中間ノード(ルートを含む)
	//for (auto& cidx : ik.nodeIdx) {
	//	//bonePositions.emplace_back(XMVector3Transform(XMLoadFloat3(&_boneNodeAddressArray[cidx]->startPos),
	//		//_boneMatrices[cidx] ));
	//	bonePositions.push_back(XMLoadFloat3(&_boneNodeAddressArray[cidx]->startPos));
	//}

	//vector<XMMATRIX> mats(bonePositions.size());
	//fill(mats.begin(), mats.end(), XMMatrixIdentity());
	////ちょっとよくわからないが、PMDエディタの6.8°が0.03になっており、これは180で割っただけの値である。
	////つまりこれをラジアンとして使用するにはXM_PIを乗算しなければならない…と思われる。
	//auto ikLimit = ik.limit * XM_PI;
	////ikに設定されている試行回数だけ繰り返す
	//for (int c = 0; c < ik.iterations; ++c) {
	//	//ターゲットと末端がほぼ一致したら抜ける
	//	if (XMVector3Length(XMVectorSubtract(endPos, targetNextPos)).m128_f32[0] <= epsilon) {
	//		break;
	//	}
	//	//それぞれのボーンを遡りながら角度制限に引っ掛からないように曲げていく
	//	for (int bidx = 0; bidx < bonePositions.size(); ++bidx) {
	//		const auto& pos = bonePositions[bidx];

	//		//まず現在のノードから末端までと、現在のノードからターゲットまでのベクトルを作る
	//		auto vecToEnd = XMVectorSubtract(endPos, pos);
	//		auto vecToTarget = XMVectorSubtract(targetNextPos, pos);
	//		vecToEnd = XMVector3Normalize(vecToEnd);
	//		vecToTarget = XMVector3Normalize(vecToTarget);

	//		//ほぼ同じベクトルになってしまった場合は外積できないため次のボーンに引き渡す
	//		if (XMVector3Length(XMVectorSubtract(vecToEnd, vecToTarget)).m128_f32[0] <= epsilon) {
	//			continue;
	//		}
	//		//外積計算および角度計算
	//		auto cross = XMVector3Normalize(XMVector3Cross(vecToEnd, vecToTarget));
	//		float angle = XMVector3AngleBetweenVectors(vecToEnd, vecToTarget).m128_f32[0];
	//		angle = min(angle, ikLimit);//回転限界補正
	//		XMMATRIX rot = XMMatrixRotationAxis(cross, angle);//回転行列
	//		//posを中心に回転
	//		auto mat = XMMatrixTranslationFromVector(-pos) *
	//			rot *
	//			XMMatrixTranslationFromVector(pos);
	//		mats[bidx] *= mat;//回転行列を保持しておく(乗算で回転重ね掛けを作っておく)
	//		//対象となる点をすべて回転させる(現在の点から見て末端側を回転)
	//		for (auto idx = bidx - 1; idx >= 0; --idx) {//自分を回転させる必要はない
	//			bonePositions[idx] = XMVector3Transform(bonePositions[idx], mat);
	//		}
	//		endPos = XMVector3Transform(endPos, mat);
	//		//もし正解に近くなってたらループを抜ける
	//		if (XMVector3Length(XMVectorSubtract(endPos, targetNextPos)).m128_f32[0] <= epsilon) {
	//			break;
	//		}
	//	}
	//}
	//int idx = 0;
	//for (auto& cidx : ik.nodeIdx) {
	//	boneMatrices[cidx] = mats[idx];
	//	++idx;
	//}
	//auto node = _boneNodeAddressArray[ik.nodeIdx.back()];
	//recursiveMatrixMultiply(node, parentMat);

}
