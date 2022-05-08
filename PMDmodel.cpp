#include "PMDmodel.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include<fstream>
#include<sstream>
#include<vector>
#include <map>
//#include "pmdObject3D.h"
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

//静的メンバの実体
ID3D12Device* PMDmodel::device = nullptr;
//デスクリプタサイズ
UINT PMDmodel::descriptorHandleIncrementSize;
//デスクリプタヒープ
ComPtr<ID3D12DescriptorHeap> PMDmodel::descHeap;

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

PMDmodel::PMDmodel(ID3D12Device* device, const char* filepath, PMDobject& object) :
	_object(object)
{
	// 再初期化チェック
	assert(!PMDmodel::device);

	// nullptrチェック
	assert(device);

	PMDmodel::device = device;

	transform.world = XMMatrixIdentity();
	LoadPMDFile(filepath);
	CreateTransform();
	CreateMaterial();
	CreateMaterialAndTextureView();
}

PMDmodel::~PMDmodel()
{
}

void PMDmodel::Update()
{
	//MaterialForHlsl* mapmatrix = nullptr;
	//HRESULT result;

	//result = PMDconstBuffB1->Map(0, nullptr, (void**)&mapmatrix);//マップ
	//PMDconstBuffB1->SetName(L"PMDconstBuff");
	//if (FAILED(result)) {
	//	assert(0);
	//}

	////データ転送
	////mapmatrix->alpha = material.material.alpha;
	//mapmatrix->ambient = material.material.ambient;
	//mapmatrix->diffuse = material.material.diffuse;
	//mapmatrix->specular = material.material.specular;
	////mapmatrix->specularStrength = material.material.specularStrength;
	//PMDconstBuffB1->Unmap(0, nullptr);
		// スケール、回転、平行移動行列の計算
	XMMATRIX matScale, matRot, matTrans;
	HRESULT result;
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
	result = transformBuff->Map(0, nullptr, (void**)&mappedTransform);
	if (FAILED(result)) { assert(0); }
	*mappedTransform = transform;

	transformBuff->Unmap(0, nullptr);
}

void PMDmodel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptrチェック
	assert(device);
	assert(cmdList);

	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	ID3D12DescriptorHeap* transHeaps[] = { transformHeap.Get() };
	cmdList->SetDescriptorHeaps(1, transHeaps);
	auto transH = transformHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetGraphicsRootDescriptorTable(1, transH);

	ID3D12DescriptorHeap* mdh[] = { materialDescHeap.Get() };
	cmdList->SetDescriptorHeaps(1, mdh);

	auto materialH = materialDescHeap->GetGPUDescriptorHandleForHeapStart();
	unsigned int idxOffset = 0;
	auto cbvsrvIncSize = device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 4;
	for (auto& m : materials) {

		cmdList->SetGraphicsRootDescriptorTable(2, materialH);
		cmdList->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);

		//ヒープポインターとインデックスを次に進める
		materialH.ptr += cbvsrvIncSize;
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
	fclose(fp);

	return S_OK;
}

HRESULT PMDmodel::CreateTransform()
{
	auto buffSize = sizeof(Transform);
	buffSize = (buffSize + 0xff) & ~0xff;

	auto result = device->CreateCommittedResource(
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
	result = transformBuff->Map(0, nullptr, (void**)&mappedTransform);
	if (FAILED(result)) { assert(0); }
	*mappedTransform = transform;

	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};;
	transformDescHeapDesc.NumDescriptors = 1;
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	result = device->CreateDescriptorHeap(
		&transformDescHeapDesc,
		IID_PPV_ARGS(transformHeap.ReleaseAndGetAddressOf())
	);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = transformBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = buffSize;
	device->CreateConstantBufferView(
		&cbvDesc,
		transformHeap->GetCPUDescriptorHandleForHeapStart());

	return S_OK;
}

HRESULT PMDmodel::CreateMaterial()
{
	auto materiaBuffSize = sizeof(MaterialForHlsl);
	materiaBuffSize = (materiaBuffSize + 0xff) & ~0xff;

	auto result = device->CreateCommittedResource(
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

HRESULT PMDmodel::CreateMaterialAndTextureView()
{
	auto materiaBuffSize = sizeof(MaterialForHlsl);
	materiaBuffSize = (materiaBuffSize + 0xff) & ~0xff;

	D3D12_DESCRIPTOR_HEAP_DESC matDescHeapDesc = {};
	matDescHeapDesc.NumDescriptors = materialNum * 4;
	matDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	matDescHeapDesc.NodeMask = 0;

	matDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	auto result = device->CreateDescriptorHeap(
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
	auto matDescHeapH = materialDescHeap->GetCPUDescriptorHandleForHeapStart();

	auto inc = device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (int i = 0; i < materialNum; ++i) {
		//マテリアル用定数バッファビュー
		device->CreateConstantBufferView(&matCBVDesc, matDescHeapH);
		matDescHeapH.ptr += inc;
		matCBVDesc.BufferLocation += materiaBuffSize;

		//シェーダーリソースビュー
		if (textureResources[i] == nullptr) {
			srvDesc.Format = CreateWhiteTexture()->GetDesc().Format;
			device->CreateShaderResourceView(
				CreateWhiteTexture(), &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = textureResources[i]->GetDesc().Format;
			device->CreateShaderResourceView(
				textureResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += inc;

		//乗算スフィアマップ用ビュー
		if (sphResources[i] == nullptr) {
			srvDesc.Format = CreateWhiteTexture()->GetDesc().Format;
			device->CreateShaderResourceView(
				CreateWhiteTexture(), &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = sphResources[i]->GetDesc().Format;
			device->CreateShaderResourceView(
				sphResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += inc;

		//加算スフィアマップ用ビュー
		if (spaResources[i] == nullptr) {
			srvDesc.Format = CreateBlackTexture()->GetDesc().Format;
			device->CreateShaderResourceView(
				CreateBlackTexture(), &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = spaResources[i]->GetDesc().Format;
			device->CreateShaderResourceView(
				spaResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += inc;

	}

	return S_OK;
}
