#include "SceneEffect.h"
#include <DirectXTex.h>

#include "dx12Wrapper.h"

Wrapper* SceneEffect::dx12 = nullptr;

const float SceneEffect::radius = 5.0f;				// 底面の半径
const float SceneEffect::prizmHeight = 8.0f;			// 柱の高さ
ID3D12Device* SceneEffect::device = nullptr;
UINT SceneEffect::descriptorHandleIncrementSize = 0;
ID3D12GraphicsCommandList* SceneEffect::cmdList = nullptr;
ComPtr<ID3D12RootSignature> SceneEffect::rootsignature;
ComPtr<ID3D12PipelineState> SceneEffect::pipelinestate;
ComPtr<ID3D12DescriptorHeap> SceneEffect::descHeap;
ComPtr<ID3D12Resource> SceneEffect::vertBuff;
//ComPtr<ID3D12Resource> Object3d::indexBuff;
ComPtr<ID3D12Resource> SceneEffect::texbuff;
CD3DX12_CPU_DESCRIPTOR_HANDLE SceneEffect::cpuDescHandleSRV;
CD3DX12_GPU_DESCRIPTOR_HANDLE SceneEffect::gpuDescHandleSRV;
XMMATRIX SceneEffect::matView{};
XMMATRIX SceneEffect::matProjection{};
XMMATRIX SceneEffect::matBillboard = XMMatrixIdentity();
XMMATRIX SceneEffect::matBillboardY = XMMatrixIdentity();
SpriteCommon SceneEffect::spritecommon;
Camera* SceneEffect::camera = nullptr;

bool SceneEffect::staticInitalize(ID3D12Device* _dev, SIZE ret)
{
	assert(_dev);
	SceneEffect::device = _dev;
	descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//パイプライン生成
	LoadHlsls::LoadHlsl_VS(ShaderNo::OBJ, L"Resources/shaders/OBJVertexShader.hlsl", "main", "vs_5_0");
	LoadHlsls::LoadHlsl_PS(ShaderNo::OBJ, L"Resources/shaders/OBJPixelShader.hlsl", "main", "ps_5_0");
	LoadHlsls::createPipeline(device, ShaderNo::OBJ);

	//パイプライン生成
	{
		HRESULT result = S_FALSE;
		ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
		ComPtr<ID3DBlob> gsBlob;	// ジオメトリシェーダオブジェクト
		ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
		ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

		// 頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/shaders/ParticleVS.hlsl",	// シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		// ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/shaders/ParticleGS.hlsl",	// シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "gs_5_0",	// エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&gsBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		// ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/shaders/ParticlePS.hlsl",	// シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		// 頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{ // xy座標(1行で書いたほうが見やすい)
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			},
			//{ // 法線ベクトル(1行で書いたほうが見やすい)
			//	"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			//	D3D12_APPEND_ALIGNED_ELEMENT,
			//	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			//},
			{ // uv座標(1行で書いたほうが見やすい)
				"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			},
		};

		// グラフィックスパイプラインの流れを設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

		// サンプルマスク
		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
		// ラスタライザステート
		gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		// デプスステンシルステート
		gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		// レンダーターゲットのブレンド設定
		D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
		blenddesc.BlendEnable = true;
		//半透明合成
		//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		//blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		//blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		//加算合成
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// ブレンドステートの設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		// 深度バッファのフォーマット
		gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		// 頂点レイアウトの設定
		gpipeline.InputLayout.pInputElementDescs = inputLayout;
		gpipeline.InputLayout.NumElements = _countof(inputLayout);

		// 図形の形状設定（三角形）
		//gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;


		gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
		gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[2] = {};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		// ルートシグネチャの設定
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> rootSigBlob;
		// バージョン自動判定のシリアライズ
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
		// ルートシグネチャの生成
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
		if (FAILED(result)) {
			return result;
		}

		gpipeline.pRootSignature = rootsignature.Get();

		// グラフィックスパイプラインの生成
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));

		if (FAILED(result)) {
			return result;
		}
	}

	return true;
}

bool SceneEffect::loadTexture(UINT texNumber, const wchar_t* fileName)
{
	// nullptrチェック
	assert(device);

	HRESULT result;
	// WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		fileName, WIC_FLAGS_NONE,
		&metadata, scratchImg);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	const Image* img = scratchImg.GetImage(0, 0, 0); // 生データ抽出

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	// テクスチャ用バッファの生成
	CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	result = device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&spritecommon._texBuff[texNumber]));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// テクスチャバッファにデータ転送
	result = spritecommon._texBuff[texNumber]->WriteToSubresource(
		0,
		nullptr, // 全領域へコピー
		img->pixels,    // 元データアドレス
		(UINT)img->rowPitch,  // 1ラインサイズ
		(UINT)img->slicePitch // 1枚サイズ
	);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	D3D12_RESOURCE_DESC resDesc = spritecommon._texBuff[texNumber]->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(spritecommon._texBuff[texNumber].Get(), //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(spritecommon._descHeap->GetCPUDescriptorHandleForHeapStart(), texNumber, descriptorHandleIncrementSize)
	);

	return true;
}

void SceneEffect::PreDraw(ID3D12GraphicsCommandList* _cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(SceneEffect::cmdList == nullptr);

	// コマンドリストをセット
	SceneEffect::cmdList = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(LoadHlsls::pipeline.at(ShaderNo::OBJ)._pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(LoadHlsls::pipeline.at(ShaderNo::OBJ)._rootsignature.Get());
	// プリミティブ形状を設定
	//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void SceneEffect::PostDraw()
{
	// コマンドリストを解除
	SceneEffect::cmdList = nullptr;
}

void SceneEffect::UpdateViewMatrix()
{
	XMVECTOR eyePosition = XMLoadFloat3(&camera->GetEye());
	XMVECTOR targetPosition = XMLoadFloat3(&camera->GetTarget());
	XMVECTOR upVector = XMLoadFloat3(&camera->GetUp());

	///カメラのZ軸(視線方向)
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	//０ベクトルを除外
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));
	//ベクトルを正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	///カメラのX軸(右方向)
	XMVECTOR cameraAxisX;
	//X軸は上方向とZ軸の外積で求める
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	//ベクトルを正規化
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	///カメラのY軸(上方向)
	XMVECTOR cameraAxisY;
	//Y軸はZ軸とX軸の外積で求める
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
	//ベクトルを正規化
	cameraAxisY = XMVector3Normalize(cameraAxisY);

	///カメラ回転行列
	XMMATRIX matCameraRot;
	//カメラ座標系からワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	///位置により逆行列を計算
	matView = XMMatrixTranspose(matCameraRot);

	//視点座標に-1を掛けた座標
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	//カメラの位置からワールド原点へのベクトル(カメラ座標系)
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);
	//1つのベクトルにまとめる
	XMVECTOR transration = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[3], 1.0f);

	//ビュー行列に平行移動成分を設定
	matView.r[3] = transration;

#pragma region 全方向ビルボード
	//ビルボード行列
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma endregion

	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	ybillCameraAxisX = cameraAxisX;
	ybillCameraAxisY = XMVector3Normalize(upVector);
	ybillCameraAxisZ = XMVector3Cross(
		cameraAxisX, cameraAxisY);

	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);

}

SceneEffect* SceneEffect::Create(UINT texNumber, XMFLOAT3 position, XMFLOAT4 color, XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	// 仮サイズ
	XMFLOAT2 size = { 100.0f, 100.0f };

	if (spritecommon._texBuff[texNumber])
	{
		// テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = spritecommon._texBuff[texNumber]->GetDesc();
		// スプライトのサイズをテクスチャのサイズに設定
		size = { (float)resDesc.Width, (float)resDesc.Height };
	}

	//インスタンスを生成
	SceneEffect* instance = new SceneEffect(texNumber, position, size, color, anchorpoint, isFlipX, isFlipY);
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initalize()) {
		delete instance;
		assert(0);
		return nullptr;
	}

	return instance;
}

SceneEffect::SceneEffect(UINT texNumber, XMFLOAT3 position, XMFLOAT2 size, XMFLOAT4 color, XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	this->position = position;
	this->size = size;
	this->anchorpoint = anchorpoint;
	this->matWorld = XMMatrixIdentity();
	this->color = color;
	this->texNumber = texNumber;
	this->isFlipX = isFlipX;
	this->isFlipY = isFlipY;
	this->texSize = size;
}

bool SceneEffect::Initalize()
{
	// nullptrチェック
	assert(device);

	HRESULT result;
	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	return true;
}

void SceneEffect::Update()
{
	// 頂点バッファへデータ転送
	VertexPos* vertMap = nullptr;
	auto result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		for (std::forward_list<sceneEffect>::iterator it = effects.begin(); it != effects.end(); it++) {
			vertMap->pos = it->position;
			vertMap->scale = it->scale;
			vertMap++;
		}
		vertBuff->Unmap(0, nullptr);
	}
	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();

	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->matBillboard = matBillboard;
	constMap->mat = matViewProjection;
	if (SUCCEEDED(result)) {
		for (std::forward_list<sceneEffect>::iterator it = effects.begin(); it != effects.end(); it++) {
			constMap->color = it->color;
			//constMap++;
		}
	}
	constBuff->Unmap(0, nullptr);
}

void SceneEffect::Draw()
{
	// nullptrチェック
	assert(device);
	assert(SceneEffect::cmdList);

	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファの設定
	//cmdList->IASetIndexBuffer(&ibView);

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);
	// 描画コマンド
	//cmdList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);
	//cmdList->DrawInstanced(_countof(vertices), 1, 0, 0);
	cmdList->DrawInstanced((UINT)std::distance(effects.begin(), effects.end()), 1, 0, 0);
}

void SceneEffect::CreateParticle(XMFLOAT3 emitter, XMFLOAT4 color)
{
	effects.emplace_front();

	sceneEffect& p = effects.front();

	p.position = emitter;
	p.scale = 100.0f;
	p.color = color;
}

