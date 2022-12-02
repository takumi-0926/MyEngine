#pragma once

#include <DirectXMath.h>

/// <summary>
/// 〇影
/// </summary>
class CircleShadow {
private://エイリアス
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData {
		XMVECTOR				dir;//投影方向
		XMFLOAT3		  casterPos;//シャドウキャスター
		float	distanceCasterLight;//キャスターライト間距離
		XMFLOAT3			  atten;//距離減衰係数
		float				   pad3;//パディング（４バイト）
		XMFLOAT2	 factorAngleCos;//角度減衰係数
		unsigned int		 active;//アクティブフラグ
		float				   pad4;//パディング（４バイト）
	};

public://メンバ関数
	//投影方向セット
	inline void SetDir(const XMVECTOR& dir) { this->dir = DirectX::XMVector3Normalize(dir); }
	//投影方向取得
	inline const XMVECTOR& GetDir() { return dir; }
	//シャドウキャスターセット
	inline void SetCasterPos(const XMFLOAT3& casterPos) { this->casterPos = casterPos; }
	//シャドウキャスター取得
	inline const XMFLOAT3& GetCasterPos() { return casterPos; }
	//キャスターライト間距離セット
	inline void SetDistanceCasterPos(float distanceCasterLight) { this->distanceCasterLight = distanceCasterLight; }
	//キャスターライト間距離取得
	inline float GetDistanceCasterPos() { return distanceCasterLight; }
	//距離減衰係数セット
	inline void SetAtten(const XMFLOAT3& atten) { this->atten = atten; }
	//距離減衰係数取得
	inline const XMFLOAT3& GetAtten() { return atten; }
	//角度減衰係数セット
	inline void SetFactorAngle(const XMFLOAT2& factorAngle) {
		this->factorAngleCos.x = cosf(DirectX::XMConvertToRadians(factorAngle.x));
		this->factorAngleCos.y = cosf(DirectX::XMConvertToRadians(factorAngle.y));
	}
	//角度減衰係数取得
	inline const XMFLOAT2& GetFactorAngleCos() { return factorAngleCos; }
	//アクティブフラグセット
	inline void SetActive(bool active) { this->active = active; }
	//アクティブフラグ取得
	inline bool IsActive() { return active; }

private://メンバ変数
	//方向
	XMVECTOR dir = { 1,0,0,0 };
	//キャスターライト間距離
	float distanceCasterLight = 1000.0f;
	//キャスターライト座標
	XMFLOAT3 casterPos = { 0,0,0 };
	//距離減衰係数
	XMFLOAT3 atten = { 0.5f,0.6f,0.0f };
	//減衰角度
	XMFLOAT2 factorAngleCos = { 0.2f,0.5f };
	//有効フラグ
	bool active = false;
};