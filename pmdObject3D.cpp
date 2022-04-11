#include "pmdObject3D.h"

ComPtr<ID3D12RootSignature> PMDobject::_rootsignature;
ComPtr<ID3D12PipelineState> PMDobject::_pipelinestate;

bool PMDobject::StaticInitialize(ID3D12Device* device, SIZE ret)
{
	//assert(!PMDobject::device);

	assert(device);

	PMDobject::device = device;

	InitializeGraphicsPipeline();

    return true;
}

bool PMDobject::InitializeGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;	// �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	result = D3DCompileFromFile(//VS
		L"Resources/shaders/BasicVertexShader.hlsl",//�V�F�[�_�[��
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS", "vs_5_0",//�֐��A�ΏۃV�F�[�_�[
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
			return 0;//exit()
		}
		else {
			string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());//�f�[�^��\��
		}
	}

	result = D3DCompileFromFile(//PS
		L"Resources/shaders/BasicPixelShader.hlsl",//�V�F�[�_�[��
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS", "ps_5_0",//�֐��A�ΏۃV�F�[�_�[
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
			return 0;//exit()
		}
		else {
			string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());//�f�[�^��\��
		}
	}
#pragma endregion

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{//���_���W
		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//�@��
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//uv
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
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
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//�T���v���}�X�N,���X�^���C�U�[
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�f�t�H���g�̃T���v���}�X�N��\���萔
	gpipeline.RasterizerState.MultisampleEnable = false;//�A���`�G�C���A�X�i�܂��g��Ȃ��j
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//���g��h��Ԃ�
	gpipeline.RasterizerState.DepthClipEnable = true;//�[�x�����̃N���b�s���O�L��

	//�����_�^�[�Q�b�g�̐ݒ�
	gpipeline.NumRenderTargets = 1;//����1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;//0�`1�ɐ��K�����ꂽRGBA

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

	//���[�g�V�O�l�`��
	//ID3D12RootSignature* rootsignature;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//�f�X�N���v�^�����W
	D3D12_DESCRIPTOR_RANGE descTblRange[3] = {};
	descTblRange[0].NumDescriptors = 1;//�萔1��
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//���(�萔)
	descTblRange[0].BaseShaderRegister = 0;//�O�ԃX���b�g����
	descTblRange[0].OffsetInDescriptorsFromTableStart
		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descTblRange[1].NumDescriptors = 1;//�萔2��
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//���(�萔)
	descTblRange[1].BaseShaderRegister = 1;//1�ԃX���b�g����
	descTblRange[1].OffsetInDescriptorsFromTableStart
		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descTblRange[2].NumDescriptors = 3;//�e�N�X�`��2��
	descTblRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//���(�e�N�X�`��)
	descTblRange[2].BaseShaderRegister = 0;//�O�ԃX���b�g����
	descTblRange[2].OffsetInDescriptorsFromTableStart
		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//���[�g�p�����[�^�[
	D3D12_ROOT_PARAMETER rootparam[2] = {};
	//rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootparam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
	//rootparam[0].DescriptorTable.NumDescriptorRanges = 1;
	//rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����

	//�{���̂����ł͂Ȃ���[
	rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootparam[0].Descriptor.RegisterSpace = 0;
	rootparam[0].Descriptor.ShaderRegister = 0;
	rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����

	rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];
	rootparam[1].DescriptorTable.NumDescriptorRanges = 2;
	rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����

	rootSignatureDesc.pParameters = rootparam;//���[�g�p�����[�^�[�̐擪�A�h���X
	rootSignatureDesc.NumParameters = 2;//���[�g�p�����[�^�[��

	//�T���v���[
	D3D12_STATIC_SAMPLER_DESC sampleDesc = {};
	sampleDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�������̌J��Ԃ�
	sampleDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�����̌J��Ԃ�
	sampleDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s���̌J��Ԃ�
	sampleDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�͍�
	sampleDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;//���`���
	sampleDesc.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
	sampleDesc.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
	sampleDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_���猩����
	sampleDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//���T���v�����O���Ȃ�

	rootSignatureDesc.pStaticSamplers = &sampleDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	if (FAILED(result)) {
		return result;
	}

	result = device->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&_rootsignature));
	if (FAILED(result)) {
		return result;
	}

	//rootSigBlob->Release();
	gpipeline.pRootSignature = _rootsignature.Get();

	//�O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐���
	//ID3D12PipelineState* _pipelinestate = nullptr;
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&_pipelinestate));
	if (FAILED(result)) {
		return result;
	}

    return true;
}

PMDobject* PMDobject::Create()
{
    //���f���̃C���X�^���X���쐬
    PMDobject* pmdObject = new PMDobject();
    if (pmdObject == nullptr) {
        return nullptr;
    }

    //������
    if (!pmdObject->Initialize()) {
        delete pmdObject;
        assert(0);
        return nullptr;
    }

    return pmdObject;
}

void PMDobject::SetModel(PMDmodel* model)
{
	this->model = model;
	model->Initialize();
}

bool PMDobject::Initialize()
{
	assert(device);

	HRESULT result;

	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&PMDconstBuffB0));

    return true;
}

void PMDobject::Update()
{
	//HRESULT result;
	//XMMATRIX matScale, matRot, matTrans;

	//// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	//matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//matRot = XMMatrixIdentity();
	//matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	//matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	//matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	//matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//// ���[���h�s��̍���
	//matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	//matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	//matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	//matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	//const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	//const XMFLOAT3& cameraPos = camera->GetEye();

	//// �萔�o�b�t�@�փf�[�^�]��(OBJ)
	//ConstBufferDataB0* constMap = nullptr;
	//result = PMDconstBuffB0->Map(0, nullptr, (void**)&constMap);
	//if (FAILED(result)) {
	//	assert(0);
	//}

	////constMap->mat = matWorld * matView * matProjection;	// �s��̍���
	//constMap->viewproj = matViewProjection;
	//constMap->world = matWorld;
	//constMap->cameraPos = cameraPos;
	//PMDconstBuffB0->Unmap(0, nullptr);

	model->Update();
}

void PMDobject::Draw()
{
	// nullptr�`�F�b�N
	assert(device);
	assert(cmdList);

	cmdList->IASetVertexBuffers(0, 1, &model->VbView());
	cmdList->IASetIndexBuffer(&model->IbView());

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(_pipelinestate.Get());

	cmdList->SetGraphicsRootSignature(_rootsignature.Get());

	cmdList->SetGraphicsRootConstantBufferView(0, PMDconstBuffB0->GetGPUVirtualAddress());//�{���̂����ł͂Ȃ���[

	model->Draw(PMDobject::cmdList);

	auto materialH = model->DescHeap()->GetGPUDescriptorHandleForHeapStart();
	unsigned int idxOffset = 0;
	auto cbvsrvIncSize = device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 4;
	for (auto& m : model->Materials()) {

		cmdList->SetGraphicsRootDescriptorTable(2, materialH);
		cmdList->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);

		//�q�[�v�|�C���^�[�ƃC���f�b�N�X�����ɐi�߂�
		materialH.ptr += cbvsrvIncSize;
		idxOffset += m.indicesNum;
	}

}
