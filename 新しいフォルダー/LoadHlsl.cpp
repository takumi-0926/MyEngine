#include "LoadHlsl.h"

void LoadHlsls::LoadHlsl(
	LPCUWSTR namePath,
	ID3DBlob** objBlob,
	const char* entryPathName,
	const char* modelPathName)
{
	//指定(namePath)のhlslファイルをロード
	auto result = D3DCompileFromFile(
		namePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPathName, modelPathName,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		objBlob, &_errorBlob);
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
}