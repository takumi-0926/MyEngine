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

//静的メンバの実体
ID3D12Device* PMDmodel::device = nullptr;
//デスクリプタサイズ
UINT PMDmodel::descriptorHandleIncrementSize;
//コマンドリスト
//ID3D12GraphicsCommandList* PMDmodel::cmdList = nullptr;
//デスクリプタヒープ
ComPtr<ID3D12DescriptorHeap> PMDmodel::descHeap;

ComPtr<ID3D12DescriptorHeap> PMDmodel::materialDescHeap;

//PMDヘッダ
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
	fill(data.begin(), data.end(), 0xff);//全部255で埋める

	//データ転送
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
	fill(data.begin(), data.end(), 0x00);//全部255で埋める

	//データ転送
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
	// 再初期化チェック
	assert(!PMDmodel::device);

	// nullptrチェック
	assert(device);

	PMDmodel::device = device;

	// デスクリプタヒープの初期化
	InitializeDescriptorHeap();

	return true;
}

bool PMDmodel::Initialize()
{
	// nullptrチェック
	assert(device);

	//HRESULT result;
	//// 定数バッファの生成
	//result = _dev->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
	//	D3D12_HEAP_FLAG_NONE,
	//	&CD3DX12_RESOURCE_DESC::Buffer((sizeof(MatricesData) + 0xff) & ~0xff),
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&PMDconstBuffB1));

	//マテリアルバッファの作成
	auto materiaBuffSize = sizeof(MaterialForHlsl);
	materiaBuffSize = (materiaBuffSize + 0xff) & ~0xff;

	HRESULT result;

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(materiaBuffSize * materialNum),//メモリがもったいない
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

	result = PMDconstBuffB0->Map(0, nullptr, (void**)&mapmatrix);//マップ
	if (FAILED(result)) {
		assert(0);
	}

	//データ転送
	mapmatrix->alpha = material.material.alpha;
	mapmatrix->ambient = material.material.ambient;
	mapmatrix->diffuse = material.material.diffuse;
	mapmatrix->specular = material.material.specular;
	mapmatrix->specularStrength = material.material.specularStrength;
	PMDconstBuffB0->Unmap(0, nullptr);
}

void PMDmodel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptrチェック
	assert(device);
	assert(cmdList);

	//cmdList->SetGraphicsRootSignature(rootsignature);
	cmdList->SetGraphicsRootConstantBufferView(0, PMDconstBuffB0->GetGPUVirtualAddress());//本来のやり方ではないよー
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

	//	//ヒープポインターとインデックスを次に進める
	//	materialH.ptr += cbvsrvIncSize;
	//	idxOffset += m.indicesNum;
	//}
}

void PMDmodel::CreateModel(const std::string& strModelPath)
{
	HRESULT result;
	FILE* fp;
	char signature[3] = {};//シグネチャ
	//string strModelPath = "Model/初音ミクmetal.pmd";
	result = fopen_s(&fp, strModelPath.c_str(), "rb");//ファイルを開く
	if (FAILED(result)) {
		assert(0);
	}

	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	unsigned int vertnum;//頂点数
	fread(&vertnum, sizeof(vertnum), 1, fp);

	constexpr size_t pmdvertex_size = 38;//頂点当たりのサイズ
	vector<unsigned char>vertices(vertnum * pmdvertex_size);//バッファの確保
	fread(vertices.data(), vertices.size(), 1, fp);//読み込み

#pragma region 頂点バッファ
//頂点バッファーの生成 : WriteSubResourceで転送するためのヒープ設定()
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

	//頂点バッファのリソース生成
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

	//頂点情報のコピー// 4/1
	unsigned char* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	vertBuff->Unmap(0, nullptr);//マップ解除

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

#pragma region インデックスの実装
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

	//マテリアルの読み込み
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

		//モデルとテクスチャパスからアプリケーションからのテクスチャパスを得る
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
	//コピー
	for (int i = 0; i < pmdMaterials.size(); i++) {
		materials[i].indicesNum = pmdMaterials[i].indicesNum;
		materials[i].material.diffuse = pmdMaterials[i].diffuse;
		materials[i].material.alpha = pmdMaterials[i].alpha;
		materials[i].material.specular = pmdMaterials[i].specular;
		materials[i].material.specularStrength = pmdMaterials[i].specularStrength;
		materials[i].material.ambient = pmdMaterials[i].ambient;
	}
	//マテリアルバッファの作成
	auto materiaBuffSize = sizeof(MaterialForHlsl);
	materiaBuffSize = (materiaBuffSize + 0xff) & ~0xff;

	ID3D12Resource* materialBuff = nullptr;
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(materiaBuffSize * materialNum),//メモリがもったいない
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&materialBuff));
	if (FAILED(result)) {
		assert(0);
	}

	//マップマテリアルにコピー
	char* mapMaterial = nullptr;
	result = materialBuff->Map(0, nullptr, (void**)&mapMaterial);

	for (auto& m : materials) {
		*((MaterialForHlsl*)mapMaterial) = m.material;//データコピー
		mapMaterial += materiaBuffSize;//次のアライメント位置まで
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

	//通常テクスチャビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//RGBA(正規化)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;//
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;//ミップマップは使用しない

	//先頭を記録
	//auto cpuDescHandleSRV = materialDescHeap->GetCPUDescriptorHandleForHeapStart();
	cpuDescHandleSRV = materialDescHeap->GetCPUDescriptorHandleForHeapStart();
	gpuDescHandleSRV = materialDescHeap->GetGPUDescriptorHandleForHeapStart();

	auto inc = device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (int i = 0; i < materialNum; ++i) {
		//マテリアル用定数バッファビュー
		device->CreateConstantBufferView(&matCBVDesc, cpuDescHandleSRV);
		cpuDescHandleSRV.ptr += inc;
		matCBVDesc.BufferLocation += materiaBuffSize;

		//シェーダーリソースビュー
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

		//乗算スフィアマップ用ビュー
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

		//加算スフィアマップ用ビュー
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

	fclose(fp);//ファイルを閉じる
}

bool PMDmodel::InitializeDescriptorHeap()
{
	HRESULT result = S_FALSE;

	// デスクリプタヒープを生成	
	D3D12_DESCRIPTOR_HEAP_DESC descheapDesc = {};
	descheapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descheapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descheapDesc.NodeMask = 0;
	descheapDesc.NumDescriptors = 2;
	result = device->CreateDescriptorHeap(&descheapDesc, IID_PPV_ARGS(&descHeap));//生成
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// デスクリプタサイズを取得
	descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return true;
}

PMDmodel::PMDmodel()
{
}

//bool PMDmodel::InitializeGraphicsPipeline()
//{
//#pragma region hlslファイルのロード
//	ID3DBlob* vsBlob = nullptr;
//	ID3DBlob* psBlob = nullptr;
//	ID3DBlob* errorBlob = nullptr;
//
//	HRESULT result;
//
//	result = D3DCompileFromFile(//VS
//		L"Resources/shaders/BasicVertexShader.hlsl",//シェーダー名
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		"BasicVS", "vs_5_0",//関数、対象シェーダー
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
//		0,
//		&vsBlob, &errorBlob);
//	if (FAILED(result)) {
//		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
//			::OutputDebugStringA("ファイルが見当たりません");
//			return 0;//exit()
//		}
//		else {
//			string errstr;
//			errstr.resize(errorBlob->GetBufferSize());
//			copy_n((char*)errorBlob->GetBufferPointer(),
//				errorBlob->GetBufferSize(),
//				errstr.begin());
//			errstr += "\n";
//			::OutputDebugStringA(errstr.c_str());//データを表示
//		}
//	}
//
//	result = D3DCompileFromFile(//PS
//		L"Resources/shaders/BasicPixelShader.hlsl",//シェーダー名
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		"BasicPS", "ps_5_0",//関数、対象シェーダー
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
//		0,
//		&psBlob, &errorBlob);
//	if (FAILED(result)) {
//		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
//			::OutputDebugStringA("ファイルが見当たりません");
//			return 0;//exit()
//		}
//		else {
//			string errstr;
//			errstr.resize(errorBlob->GetBufferSize());
//			copy_n((char*)errorBlob->GetBufferPointer(),
//				errorBlob->GetBufferSize(),
//				errstr.begin());
//			errstr += "\n";
//			::OutputDebugStringA(errstr.c_str());//データを表示
//		}
//	}
//#pragma endregion
//
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
//	{//頂点座標
//		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	{//法線
//		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	{//uv
//		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	{//ボーン番号
//		"BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	{//ウェイト
//		"WEIGHT",0,DXGI_FORMAT_R8_UINT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	{//輪郭線フラグ
//		"EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,
//		D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//	},
//	};
//
//#pragma region パイプライン
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
//	gpipeline.pRootSignature = nullptr;//後で設定
//
//	//シェーダー
//	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
//	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
//	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
//	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();
//
//	//サンプルマスク,ラスタライザー
//	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//デフォルトのサンプルマスクを表す定数
//	gpipeline.RasterizerState.MultisampleEnable = false;//アンチエイリアス（まだ使わない）
//	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
//	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//中身を塗りつぶす
//	gpipeline.RasterizerState.DepthClipEnable = true;//深度方向のクリッピング有効
//
//	//レンダターゲットの設定
//	gpipeline.NumRenderTargets = 1;//今は1つ
//	gpipeline.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;//0～1に正規化されたRGBA
//
//	//深度ステンシル
//	gpipeline.DepthStencilState.DepthEnable = true;//使う
//	gpipeline.DepthStencilState.StencilEnable = false;//
//	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込む
//	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さい方を採用
//	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
//
//	//ブレンドステートの設定
//	gpipeline.BlendState.AlphaToCoverageEnable = false;
//	gpipeline.BlendState.IndependentBlendEnable = false;
//	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlenddesc = {};
//	renderTargetBlenddesc.BlendEnable = false;
//	renderTargetBlenddesc.LogicOpEnable = false;
//	renderTargetBlenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//	gpipeline.BlendState.RenderTarget[0] = renderTargetBlenddesc;
//
//	//入力レイアウトの設定
//	gpipeline.InputLayout.pInputElementDescs = inputLayout;//レイアウトの先頭アドレス
//	gpipeline.InputLayout.NumElements = _countof(inputLayout);//レイアウト配列の要素
//	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//カットなし
//	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形で構成
//
//	//アンチエイリアシングのためのサンプル数設定
//	gpipeline.SampleDesc.Count = 1;
//	gpipeline.SampleDesc.Quality = 0;
//#pragma endregion
//
//#pragma region ルートシグネチャ
//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
//	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//#pragma region ディスクリプタレンジ_P.206
//	D3D12_DESCRIPTOR_RANGE descTblRange[3] = {};
//	descTblRange[0].NumDescriptors = 1;//定数1つ目
//	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//種別(定数)
//	descTblRange[0].BaseShaderRegister = 0;//０番スロットから
//	descTblRange[0].OffsetInDescriptorsFromTableStart
//		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//	descTblRange[1].NumDescriptors = 1;//定数2つ目
//	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//種別(定数)
//	descTblRange[1].BaseShaderRegister = 1;//1番スロットから
//	descTblRange[1].OffsetInDescriptorsFromTableStart
//		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//	descTblRange[2].NumDescriptors = 3;//テクスチャ2つ
//	descTblRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//種別(テクスチャ)
//	descTblRange[2].BaseShaderRegister = 0;//０番スロットから
//	descTblRange[2].OffsetInDescriptorsFromTableStart
//		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//#pragma endregion
//
//#pragma region ルートパラメーター_P.207
//	D3D12_ROOT_PARAMETER rootparam[2] = {};
//	//rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	//rootparam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
//	//rootparam[0].DescriptorTable.NumDescriptorRanges = 1;
//	//rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
//
//	//本来のやり方ではないよー
//	rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//	rootparam[0].Descriptor.RegisterSpace = 0;
//	rootparam[0].Descriptor.ShaderRegister = 0;
//	rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
//
//	rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	rootparam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];
//	rootparam[1].DescriptorTable.NumDescriptorRanges = 2;
//	rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
//
//#pragma endregion
//
//	rootSignatureDesc.pParameters = rootparam;//ルートパラメーターの先頭アドレス
//	rootSignatureDesc.NumParameters = 2;//ルートパラメーター数
//
//#pragma region サンプラー
//	D3D12_STATIC_SAMPLER_DESC sampleDesc = {};
//	sampleDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横方向の繰り返し
//	sampleDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦方向の繰り返し
//	sampleDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行きの繰り返し
//	sampleDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーは黒
//	sampleDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;//線形補間
//	sampleDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
//	sampleDesc.MinLOD = 0.0f;//ミップマップ最小値
//	sampleDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダから見える
//	sampleDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//リサンプリングしない
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
//	//グラフィックスパイプラインステートオブジェクトの生成
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
//	// PreDrawとPostDrawがペアで呼ばれていなければエラー
//	assert(PMDmodel::cmdList == nullptr);
//
//	// コマンドリストをセット
//	PMDmodel::cmdList = cmdList;
//
//	// プリミティブ形状を設定
//	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//}


//void PMDmodel::postDraw()
//{
//	// コマンドリストを解除
//	PMDmodel::cmdList = nullptr;
//}


