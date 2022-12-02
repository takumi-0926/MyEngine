#pragma once

#include <DirectXMath.h>

class PointLight {
private:
	// DirectX::‚ðÈ—ª
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	struct ConstBufferData {
		XMFLOAT3 lightpos;
		float pad1;
		XMFLOAT3 lightcolor;
		float pad2;
		XMFLOAT3 lightatten;
		unsigned int active;
	};

public:

	inline void SetLightPos(const XMFLOAT3& lightpos) {
		this->lightpos = lightpos;
	}
	inline const XMFLOAT3& GetLightPos() { return lightpos; }
	inline void SetLightColor(const XMFLOAT3& lightcolor) {
		this->lightcolor = lightcolor;
	}
	inline const XMFLOAT3& GetLightColor() { return lightcolor; }
	inline void SetLightAtten(const XMFLOAT3& lightatten) {
		this->lightatten = lightatten;
	}
	inline const XMFLOAT3& GetLightAtten() { return lightatten; }
	
	inline void SetActive(bool active) {
		this->active = active;
	}
	inline bool IsActive() { return active; }

public:
	XMFLOAT3 lightpos = { 0,0,0 };

	XMFLOAT3 lightcolor = { 1,1,1 };

	XMFLOAT3 lightatten = { 1.0f,1.0f,1.0f };

	bool active = false;
};