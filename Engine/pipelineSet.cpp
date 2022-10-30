#include "pipelineSet.h"

ComPtr<ID3DBlob> LoadHlsls::_errorBlob = nullptr;
std::map<int, PipelineSet> LoadHlsls::pipeline = {};
std::map<int, ComPtr<ID3DBlob>> LoadHlsls::vsBlob = {};
std::map<int, ComPtr<ID3DBlob>> LoadHlsls::psBlob = {};

void LoadHlsls::LoadHlsl_VS(
	int Number,
	const wchar_t* namePath,
	const char* entryPathName,
	const char* modelPathName)
{
	ComPtr<ID3DBlob> shaderBolb;

	//指定(namePath)のhlslファイルをロード
	auto result = D3DCompileFromFile(
		(LPCWSTR)namePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPathName, modelPathName,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shaderBolb, &_errorBlob);
	//読み込み成功チェック
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("ファイルが見当たりません");
		}
		else {
			string errstr;
			errstr.resize(_errorBlob->GetBufferSize());
			copy_n((char*)_errorBlob->GetBufferPointer(),
				_errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());//データを表示
		}
	}
	vsBlob.insert(std::make_pair(Number, shaderBolb));
}

void LoadHlsls::LoadHlsl_PS(
	int Number,
	const wchar_t* namePath,
	const char* entryPathName,
	const char* modelPathName)
{
	ComPtr<ID3DBlob> shaderBolb;

	//指定(namePath)のhlslファイルをロード
	auto result = D3DCompileFromFile(
		(LPCWSTR)namePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPathName, modelPathName,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shaderBolb, &_errorBlob);
	//読み込み成功チェック
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("ファイルが見当たりません");
		}
		else {
			string errstr;
			errstr.resize(_errorBlob->GetBufferSize());
			copy_n((char*)_errorBlob->GetBufferPointer(),
				_errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());//データを表示
		}
	}
	psBlob.insert(std::make_pair(Number, shaderBolb));
}

void LoadHlsls::createPipeline(ID3D12Device* _dev, int Number)
{
	HRESULT result;
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト
	ComPtr<ID3DBlob> rootSigBlob;
	PipelineSet pipelineset;

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//ボーン番号
			"BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{//ウェイト
			"WEIGHT",0,DXGI_FORMAT_R8_UINT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{//輪郭線フラグ
			"EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};
	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	CD3DX12_ROOT_SIGNATURE_DESC _rootSignatureDesc = {};

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//2Dオブジェクト用
	if (Number == ShaderNo::TWO) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::TWO).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::TWO).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// ブレンドステートの設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// バージョン自動判定のシリアライズ
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//objファイル用
	else if (Number == ShaderNo::OBJ) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::OBJ).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::OBJ).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// ブレンドステートの設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// バージョン自動判定のシリアライズ
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//fbxファイル用
	else if (Number == ShaderNo::FBX) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::FBX).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::FBX).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA全てのチャンネルを描画
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// ブレンドステートの設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		// 頂点レイアウトの設定
		gpipeline.InputLayout.pInputElementDescs = inputLayout;
		gpipeline.InputLayout.NumElements = _countof(inputLayout);

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		gpipeline.NumRenderTargets = 1;    // 描画対象は1つ
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
		gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[3];
		// CBV（座標変換行列用）
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV（テクスチャ）
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		//CBV（スキニング）
		rootparams[2].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// バージョン自動判定のシリアライズ
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//pmdファイル用
	else if (Number == ShaderNo::PMD) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::PMD).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::PMD).Get());

		//サンプルマスク,ラスタライザー
		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//デフォルトのサンプルマスクを表す定数
		gpipeline.RasterizerState.MultisampleEnable = false;//アンチエイリアス（まだ使わない）
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
		gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//中身を塗りつぶす
		gpipeline.RasterizerState.DepthClipEnable = true;//深度方向のクリッピング有効

		//レンダターゲットの設定
		gpipeline.NumRenderTargets = 1;//今は1つ
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～1に正規化されたRGBA

		//深度ステンシル
		gpipeline.DepthStencilState.DepthEnable = true;//使う
		gpipeline.DepthStencilState.StencilEnable = false;//
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込む
		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さい方を採用
		gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		//ブレンドステートの設定
		gpipeline.BlendState.AlphaToCoverageEnable = false;
		gpipeline.BlendState.IndependentBlendEnable = false;
		D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlenddesc = {};
		renderTargetBlenddesc.BlendEnable = false;
		renderTargetBlenddesc.LogicOpEnable = false;
		renderTargetBlenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		gpipeline.BlendState.RenderTarget[0] = renderTargetBlenddesc;

		//入力レイアウトの設定
		gpipeline.InputLayout.pInputElementDescs = inputLayout;//レイアウトの先頭アドレス
		gpipeline.InputLayout.NumElements = _countof(inputLayout);//レイアウト配列の要素
		gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//カットなし
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形で構成

		//アンチエイリアシングのためのサンプル数設定
		gpipeline.SampleDesc.Count = 1;
		gpipeline.SampleDesc.Quality = 0;

		//レンジ
		CD3DX12_DESCRIPTOR_RANGE  descTblRanges[4] = {};//テクスチャと定数の２つ
		descTblRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//定数[b0](ビュープロジェクション用)
		descTblRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);//定数[b1](ワールド、ボーン用)
		descTblRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);//定数[b2](マテリアル用)
		descTblRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);//テクスチャ４つ(基本とsphとspaとトゥーン)

		//ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootParams[3] = {};
		rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootParams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

		rootParams[0].InitAsDescriptorTable(1, &descTblRanges[0]);//ビュープロジェクション変換
		rootParams[1].InitAsDescriptorTable(1, &descTblRanges[1]);//ワールド・ボーン変換
		rootParams[2].InitAsDescriptorTable(2, &descTblRanges[2]);//マテリアル周り

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};
		samplerDescs[0].Init(0);
		samplerDescs[1].Init(1, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

		_rootSignatureDesc.Init(3, rootParams, 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// バージョン自動判定のシリアライズ
		result = D3D12SerializeRootSignature(&_rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob, &errorBlob);
	}
	//単色シェーダ用
	else if (Number == ShaderNo::SINGLE) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::SINGLE).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::SINGLE).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// ブレンドステートの設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// バージョン自動判定のシリアライズ
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//ADSシェーダ用
	else if (Number == ShaderNo::PHONG) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::PHONG).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::PHONG).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// ブレンドステートの設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// バージョン自動判定のシリアライズ
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//トゥーンシェーダ用
	else if (Number == ShaderNo::TOON) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::TOON).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::TOON).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// ブレンドステートの設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// バージョン自動判定のシリアライズ
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//ブルームシェーダ用
	else if (Number == ShaderNo::BLOOM) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::BLOOM).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::BLOOM).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// ブレンドステートの設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// バージョン自動判定のシリアライズ
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}

	else if (Number == ShaderNo::HIT) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::HIT).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::HIT).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// ブレンドステートの設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// バージョン自動判定のシリアライズ
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}

	// ルートシグネチャの生成
	result = _dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(pipelineset._rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = pipelineset._rootsignature.Get();

	// グラフィックスパイプラインの生成
	result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelineset._pipelinestate.ReleaseAndGetAddressOf()));

	pipeline.insert(std::make_pair(Number, pipelineset));
}
