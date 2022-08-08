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

	//�w��(namePath)��hlsl�t�@�C�������[�h
	auto result = D3DCompileFromFile(
		(LPCWSTR)namePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPathName, modelPathName,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shaderBolb, &_errorBlob);
	//�ǂݍ��ݐ����`�F�b�N
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else {
			string errstr;
			errstr.resize(_errorBlob->GetBufferSize());
			copy_n((char*)_errorBlob->GetBufferPointer(),
				_errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());//�f�[�^��\��
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

	//�w��(namePath)��hlsl�t�@�C�������[�h
	auto result = D3DCompileFromFile(
		(LPCWSTR)namePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPathName, modelPathName,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shaderBolb, &_errorBlob);
	//�ǂݍ��ݐ����`�F�b�N
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else {
			string errstr;
			errstr.resize(_errorBlob->GetBufferSize());
			copy_n((char*)_errorBlob->GetBufferPointer(),
				_errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());//�f�[�^��\��
		}
	}
	psBlob.insert(std::make_pair(Number, shaderBolb));
}

void LoadHlsls::createPipeline(ID3D12Device* _dev, int Number)
{
	HRESULT result;
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g
	ComPtr<ID3DBlob> rootSigBlob;
	PipelineSet pipelineset;

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy���W(1�s�ŏ������ق������₷��)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv���W(1�s�ŏ������ق������₷��)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//�{�[���ԍ�
			"BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{//�E�F�C�g
			"WEIGHT",0,DXGI_FORMAT_R8_UINT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{//�֊s���t���O
			"EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};
	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	CD3DX12_ROOT_SIGNATURE_DESC _rootSignatureDesc = {};

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	//2D�I�u�W�F�N�g�p
	if (Number == ShaderNo::TWO) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::TWO).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::TWO).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// �u�����h�X�e�[�g�̐ݒ�
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA

		// �}�`�̌`��ݒ�i�O�p�`�j
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// �f�X�N���v�^�����W
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

		// ���[�g�p�����[�^
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// �X�^�e�B�b�N�T���v���[
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// �o�[�W������������̃V���A���C�Y
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//obj�t�@�C���p
	else if (Number == ShaderNo::OBJ) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::OBJ).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::OBJ).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// �u�����h�X�e�[�g�̐ݒ�
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA

		// �}�`�̌`��ݒ�i�O�p�`�j
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// �f�X�N���v�^�����W
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

		// ���[�g�p�����[�^
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// �X�^�e�B�b�N�T���v���[
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// �o�[�W������������̃V���A���C�Y
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//fbx�t�@�C���p
	else if (Number == ShaderNo::FBX) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::FBX).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::FBX).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA�S�Ẵ`�����l����`��
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// �u�����h�X�e�[�g�̐ݒ�
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		// ���_���C�A�E�g�̐ݒ�
		gpipeline.InputLayout.pInputElementDescs = inputLayout;
		gpipeline.InputLayout.NumElements = _countof(inputLayout);

		// �}�`�̌`��ݒ�i�O�p�`�j
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		gpipeline.NumRenderTargets = 1;    // �`��Ώۂ�1��
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA
		gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

		// �f�X�N���v�^�����W
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

		// ���[�g�p�����[�^
		CD3DX12_ROOT_PARAMETER rootparams[3];
		// CBV�i���W�ϊ��s��p�j
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		// SRV�i�e�N�X�`���j
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		//CBV�i�X�L�j���O�j
		rootparams[2].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
		// �X�^�e�B�b�N�T���v���[
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// �o�[�W������������̃V���A���C�Y
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//pmd�t�@�C���p
	else if (Number == ShaderNo::PMD) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::PMD).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::PMD).Get());

		//�T���v���}�X�N,���X�^���C�U�[
		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�f�t�H���g�̃T���v���}�X�N��\���萔
		gpipeline.RasterizerState.MultisampleEnable = false;//�A���`�G�C���A�X�i�܂��g��Ȃ��j
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
		gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//���g��h��Ԃ�
		gpipeline.RasterizerState.DepthClipEnable = true;//�[�x�����̃N���b�s���O�L��

		//�����_�^�[�Q�b�g�̐ݒ�
		gpipeline.NumRenderTargets = 1;//����1��
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0�`1�ɐ��K�����ꂽRGBA

		//�[�x�X�e���V��
		gpipeline.DepthStencilState.DepthEnable = true;//�g��
		gpipeline.DepthStencilState.StencilEnable = false;//
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//��������
		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//�����������̗p
		gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		//�u�����h�X�e�[�g�̐ݒ�
		gpipeline.BlendState.AlphaToCoverageEnable = false;
		gpipeline.BlendState.IndependentBlendEnable = false;
		D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlenddesc = {};
		renderTargetBlenddesc.BlendEnable = false;
		renderTargetBlenddesc.LogicOpEnable = false;
		renderTargetBlenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		gpipeline.BlendState.RenderTarget[0] = renderTargetBlenddesc;

		//���̓��C�A�E�g�̐ݒ�
		gpipeline.InputLayout.pInputElementDescs = inputLayout;//���C�A�E�g�̐擪�A�h���X
		gpipeline.InputLayout.NumElements = _countof(inputLayout);//���C�A�E�g�z��̗v�f
		gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//�J�b�g�Ȃ�
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//�O�p�`�ō\��

		//�A���`�G�C���A�V���O�̂��߂̃T���v�����ݒ�
		gpipeline.SampleDesc.Count = 1;
		gpipeline.SampleDesc.Quality = 0;

		//�����W
		CD3DX12_DESCRIPTOR_RANGE  descTblRanges[4] = {};//�e�N�X�`���ƒ萔�̂Q��
		descTblRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//�萔[b0](�r���[�v���W�F�N�V�����p)
		descTblRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);//�萔[b1](���[���h�A�{�[���p)
		descTblRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);//�萔[b2](�}�e���A���p)
		descTblRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);//�e�N�X�`���S��(��{��sph��spa�ƃg�D�[��)

		//���[�g�p�����[�^
		CD3DX12_ROOT_PARAMETER rootParams[3] = {};
		rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootParams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

		rootParams[0].InitAsDescriptorTable(1, &descTblRanges[0]);//�r���[�v���W�F�N�V�����ϊ�
		rootParams[1].InitAsDescriptorTable(1, &descTblRanges[1]);//���[���h�E�{�[���ϊ�
		rootParams[2].InitAsDescriptorTable(2, &descTblRanges[2]);//�}�e���A������

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};
		samplerDescs[0].Init(0);
		samplerDescs[1].Init(1, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

		_rootSignatureDesc.Init(3, rootParams, 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// �o�[�W������������̃V���A���C�Y
		result = D3D12SerializeRootSignature(&_rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob, &errorBlob);
	}
	//�P�F�V�F�[�_�p
	else if (Number == ShaderNo::SINGLE) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::SINGLE).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::SINGLE).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// �u�����h�X�e�[�g�̐ݒ�
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA

		// �}�`�̌`��ݒ�i�O�p�`�j
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// �f�X�N���v�^�����W
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

		// ���[�g�p�����[�^
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// �X�^�e�B�b�N�T���v���[
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// �o�[�W������������̃V���A���C�Y
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//ADS�V�F�[�_�p
	else if (Number == ShaderNo::PHONG) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::PHONG).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::PHONG).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// �u�����h�X�e�[�g�̐ݒ�
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA

		// �}�`�̌`��ݒ�i�O�p�`�j
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// �f�X�N���v�^�����W
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

		// ���[�g�p�����[�^
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// �X�^�e�B�b�N�T���v���[
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// �o�[�W������������̃V���A���C�Y
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//�g�D�[���V�F�[�_�p
	else if (Number == ShaderNo::TOON) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::TOON).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::TOON).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// �u�����h�X�e�[�g�̐ݒ�
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA

		// �}�`�̌`��ݒ�i�O�p�`�j
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// �f�X�N���v�^�����W
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

		// ���[�g�p�����[�^
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// �X�^�e�B�b�N�T���v���[
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// �o�[�W������������̃V���A���C�Y
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}
	//�u���[���V�F�[�_�p
	else if (Number == ShaderNo::BLOOM) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::BLOOM).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::BLOOM).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// �u�����h�X�e�[�g�̐ݒ�
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA

		// �}�`�̌`��ݒ�i�O�p�`�j
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// �f�X�N���v�^�����W
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

		// ���[�g�p�����[�^
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// �X�^�e�B�b�N�T���v���[
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// �o�[�W������������̃V���A���C�Y
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}

	else if (Number == ShaderNo::HIT) {
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.at(ShaderNo::HIT).Get());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.at(ShaderNo::HIT).Get());

		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// �u�����h�X�e�[�g�̐ݒ�
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA

		// �}�`�̌`��ݒ�i�O�p�`�j
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// �f�X�N���v�^�����W
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

		// ���[�g�p�����[�^
		CD3DX12_ROOT_PARAMETER rootparams[4]{};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

		// �X�^�e�B�b�N�T���v���[
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// �o�[�W������������̃V���A���C�Y
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	}

	// ���[�g�V�O�l�`���̐���
	result = _dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(pipelineset._rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = pipelineset._rootsignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelineset._pipelinestate.ReleaseAndGetAddressOf()));

	pipeline.insert(std::make_pair(Number, pipelineset));
}
