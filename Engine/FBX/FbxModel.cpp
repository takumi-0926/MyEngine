#include "FbxModel.h"
#include "..\Singleton_Heap.h"

ID3D12Device* FbxModel::device = nullptr;

FbxModel::~FbxModel()
{
	fbxScene->Destroy();
}

void FbxModel::CreateBuffers(ID3D12Device* device)
{
	//頂点-------------------------------------------
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUvSkin) * vertices.size());

	CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	auto result = device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);

	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//頂点ビュー
	vbview.BufferLocation =
		vertBuff->GetGPUVirtualAddress();
	vbview.SizeInBytes = sizeVB;
	vbview.StrideInBytes = sizeof(vertices[0]);

	//インデックス-----------------------------------
	UINT sizeIB =
		static_cast<UINT>(sizeof(unsigned short) * indices.size());

	properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	desc = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);
	result = device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	//インデックスビュー
	ibview.BufferLocation =
		indexBuff->GetGPUVirtualAddress();
	ibview.Format = DXGI_FORMAT_R16_UINT;
	ibview.SizeInBytes = sizeIB;

	//テクスチャ------------------------------------
	const DirectX::Image* img = scrachImg.GetImage(0, 0, 0);
	assert(img);

	CD3DX12_RESOURCE_DESC texresdesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			metadata.width,
			UINT(metadata.height),
			UINT16(metadata.arraySize),
			UINT16(metadata.mipLevels)
		);

	properties = CD3DX12_HEAP_PROPERTIES(
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
		D3D12_MEMORY_POOL_L0);
	result = device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&texresdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	result = texBuff->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 1;
	result = device->CreateDescriptorHeap(
		&descHeapDesc,
		IID_PPV_ARGS(&descHeapSRV)
	);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping
		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	int ptrNum = Singleton_Heap::GetInstance()->FbxTexture;
	auto handle = Singleton_Heap::GetInstance()->GetDescHeap()->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += Singleton_Heap::GetInstance()->GetDescriptorIncrementSize() * ptrNum;

	Singleton_Heap::GetInstance()->FbxTexture++;

	device->CreateShaderResourceView(
		texBuff.Get(),
		&srvDesc,
		handle
	);

	CreateConstantBuffer();
}

void FbxModel::Update()
{
	HRESULT result;
	// 定数バッファへデータ転送
	ConstBufferDataB1* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->ambient = ambient;
		constMap->diffuse = diffuse;
		//constMap->specular = specular;
		constMap->alpha = alpha;
		constBuff->Unmap(0, nullptr);
	}
}

void FbxModel::Draw(ID3D12GraphicsCommandList* cmdList)
{

	cmdList->IASetVertexBuffers(0, 1, &vbview);

	cmdList->IASetIndexBuffer(&ibview);

	cmdList->SetGraphicsRootConstantBufferView(
		1, constBuff->GetGPUVirtualAddress()
	);

	ID3D12DescriptorHeap* ppheap[] = { Singleton_Heap::GetInstance()->GetDescHeap()};
	cmdList->SetDescriptorHeaps(_countof(ppheap), ppheap);

	auto handle = Singleton_Heap::GetInstance()->GetDescHeap()->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += Singleton_Heap::GetInstance()->GetDescriptorIncrementSize() * (Singleton_Heap::GetInstance()->FbxTexture + TextureOffset);

	cmdList->SetGraphicsRootDescriptorTable(
		2, handle
	);

	//描画
	cmdList->DrawIndexedInstanced(UINT(indices.size()), 1, 0, 0, 0);
}

void FbxModel::StaticInitialize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!FbxModel::device);

	FbxModel::device = device;
}

void FbxModel::CreateConstantBuffer()
{
	HRESULT result;
	// 定数バッファの生成
	CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff);
	result = device->CreateCommittedResource(
		&properties, 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) {
		assert(0);
	}
	Update();
}
