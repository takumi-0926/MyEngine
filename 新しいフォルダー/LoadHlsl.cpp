#include "LoadHlsl.h"

void LoadHlsls::LoadHlsl(
	LPCUWSTR namePath,
	ID3DBlob** objBlob,
	const char* entryPathName,
	const char* modelPathName)
{
	//�w��(namePath)��hlsl�t�@�C�������[�h
	auto result = D3DCompileFromFile(
		namePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPathName, modelPathName,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		objBlob, &_errorBlob);
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
}