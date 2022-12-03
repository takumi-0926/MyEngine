#pragma once
#include <DirectXMath.h>
#include <d3dx12.h>
#include <d3d12.h>

#include "DirectionalLight.h"
#include "shadow/circleShadow.h"
#include "PointLight.h"

/// <summary>
/// ライト
/// </summary>
class Light {
private://エイリアス
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // 定数
	static const int DirLightNum = 3;

	//点光源の数
	static const int PointLightNum = 3;

	//〇影の数
	static const int CircleShadowNum = 3;

public://サブクラス
	//定数バッファ用構造体
	struct ConstBufferData {
		//光の色
		XMFLOAT3 color;
		float pad;
		//並行光源用
		DirectionalLight::ConstBufferData dirRights[DirLightNum];
		// 点光源用
		PointLight::ConstBufferData pointLights[PointLightNum];
		//〇影用
		CircleShadow::ConstBufferData circleShadows[CircleShadowNum];
	};

private://静的メンバ変数
	static ID3D12Device* device;

private://メンバ変数
	ComPtr<ID3D12Resource> constBuff;

	XMFLOAT3 lightcolor = { 1,1,1 };

	DirectionalLight dirLights[DirLightNum];

	PointLight pointLights[PointLightNum];

	CircleShadow circleShadows[CircleShadowNum];

	bool dirty = false;

public://静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device"></param>
	static void StaticInitalize(ID3D12Device* device);

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static Light* Create();

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initalize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList"></param>
	/// <param name="rootParameterIndex"></param>
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	/// <summary>
	/// 定数バッファ転送処理
	/// </summary>
	void TarnsferConstBuffer();

	void DefaultLightSetting();

	/// 平行光源の有効フラグセット
	void SetDirLightActive(int index, bool active);
	/// 平行光源のライト方向セット
	void SetDirLightDir(int index, const XMVECTOR& lightdir);
	/// 平行光源のライト色セット
	void SetDirLightColor(int index, const XMFLOAT3& lightcolor);

	void SetLightColor(const XMFLOAT3& lightcolor);

	void SetPointLightActive(int index, bool active);
	void SetPointLightPos(int index, const XMFLOAT3& lightpos);
	void SetPointLightColor(int index, const XMFLOAT3& lightcolor);
	void SetPointLightAtten(int index, const XMFLOAT3& lightAtten);


	void SetCircleShadowActive(int index, bool active);
	void SetCircleShadowCasterPos(int index, const XMFLOAT3& casterPos);
	void SetCircleShadowDir(int index, const XMVECTOR& lightDir);
	void SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight);
	void SetCircleShadowAtten(int index, const XMFLOAT3& lightAtten);
	void SetCircleShadowFacterAngle(int index, const XMFLOAT2& factorAngle);
};