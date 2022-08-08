#pragma once
#include <DirectXMath.h>
#include <d3dx12.h>
#include <d3d12.h>

class Light {
private:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	struct ConstBufferData {
		XMVECTOR lightv;
		XMFLOAT3 lightcolor;
	};
private:
	static ID3D12Device* device;

public:
	static void StaticInitalize(ID3D12Device* device);

private:
	ComPtr<ID3D12Resource> constBuff;

	XMVECTOR lightdir = { 1,0,0,0 };

	XMFLOAT3 lightcolor = { 1,1,1 };

	bool dirty = false;

public:
	void Initalize();

	void Update();

	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	void TarnsferConstBuffer();

	void SetLightDir(const XMVECTOR& lightdir);

	void SetLightColor(const XMFLOAT3& lightcolor);

	static Light* Create();
};