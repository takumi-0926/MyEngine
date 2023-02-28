#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include<vector>
#include <array>
#include <unordered_map>
#include "..\includes.h"
#include "..\Camera\Camera.h"

enum vmdData {
	WAIT = 0,
	WALK,
	DASH,
	ATTACK,
	DAMAGE,
	AVOID,
};

class PMDobject;
class Wrapper;
class PMDmodel {
	friend PMDobject;
	static Wrapper* dx12;
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//using TexMetadata = DirectX::TexMetadata;
	//using ScratchImage = DirectX::ScratchImage;

	//std::省略
	using string = std::string;

private:
	//シェーダに投げられるマテリアルデータ
	struct MaterialForHlsl {
		XMFLOAT3 diffuse;//ディフューズ色
		float alpha;	//ディフューズα
		XMFLOAT3 specular;//スペキュラ色
		float specularStrength;//スペキュラの強さ（乗算値）
		XMFLOAT3 ambient;//アンビエント色
	};

	//上記以外のマテリアルデータ
	struct AdditionalMaterial {
		string texpath;
		int toonIdx = 0;
		bool edgeFlg = 0;
	};

	//全体をまとめるデータ
	struct Material {
		unsigned int indicesNum = 0;
		MaterialForHlsl material = {};
		AdditionalMaterial additional = {};
	};
	Material material;

	struct Transform {
		void* operator new(size_t size);
		XMMATRIX world;
	};

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0_1
	{
		//XMFLOAT4 color;	// 色 (RGBA)
		//XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX view;
		XMMATRIX proj;
		XMMATRIX shadow;
		XMFLOAT3 cameraPos;
	};

	struct BoneNode {
		uint32_t boneIdx = 0;
		uint32_t boneType = 0;
		uint32_t ikParentBone = 0;
		XMFLOAT3 startPos = {};
		XMFLOAT3 endPos = {};
		std::vector<BoneNode*> children;
	};

	struct Motion {
		unsigned int frameNo;
		XMVECTOR quaternion;
		XMFLOAT3 offset;
		XMFLOAT2 p1, p2;
		Motion(
			unsigned int fno,
			XMVECTOR& q,
			XMFLOAT3& ofst,
			XMFLOAT2& ip1,
			const XMFLOAT2& ip2) :
			frameNo(fno),
			quaternion(q),
			offset(ofst),
			p1(ip1),
			p2(ip2) {
		}
	};

private:
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap = nullptr;

	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;

	// 頂点バッファ,ビュー
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// インデックスバッファ,ビュー
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibView = {};

	//座標変換
	Transform transform;
	Transform* mappedTransform = nullptr;
	ComPtr<ID3D12Resource> transformBuff = nullptr;
	ComPtr<ID3D12Resource> transformMat = nullptr;
	ComPtr<ID3D12DescriptorHeap> transformHeap = nullptr;

	//マテリアル
	uint32_t materialNum;
	std::vector<Material>materials;
	ComPtr<ID3D12Resource> materialBuff;
	std::vector<ComPtr<ID3D12Resource>> textureResources;
	std::vector<ComPtr<ID3D12Resource>> sphResources;
	std::vector<ComPtr<ID3D12Resource>> spaResources;
	std::vector<ComPtr<ID3D12Resource>> toonResources;

	//ヒープ領域
	static ComPtr<ID3D12DescriptorHeap> materialDescHeap;

	//ボーン
	struct vmdMotion {
		std::unordered_map<string, std::vector<Motion>> _motionData;
		unsigned int duration = 0;
		std::vector<DirectX::XMMATRIX> invBoneMatrices;
	};
#pragma pack(1)
	struct PMDBone {
		char boneName[20];
		unsigned short parentNo;
		unsigned short nextNo;
		unsigned char  type;
		unsigned short ikBoneNo;
		XMFLOAT3       pos;
	};
#pragma pack()

	map<unsigned int, vmdMotion> motion;

	vector<PMDBone> pmdBones;
	std::vector<DirectX::XMMATRIX> boneMatrices;
	std::map<std::string, BoneNode> _boneNodeTable;
	XMMATRIX* _mappedMatrices = nullptr;
	std::unordered_map<string, std::vector<Motion>> _motionData;

	//IKボーン
	struct PMDIK {
		uint16_t boneIdx = 0;;
		uint16_t targetidx = 0;
		uint16_t iterations = 0;;
		float limit = 0.0f;
		std::vector<uint16_t> nodeIdx;
	};
	std::vector<PMDIK> _ikData;

	std::vector<string> _boneNameArray;
	std::vector<BoneNode*> _boneNodeAddressArray;

	//std::vector<XMVECTOR> positions;
	//std::array<float,2> _edgeLens;
	std::vector<uint32_t> _kneeIdxes;

	struct VMDIKEnable {
		//キーフレームがあるフレーム番号
		uint32_t freamNo = 0;;

		//名前のオン/オフフラグのマップ
		std::unordered_map<std::string, bool> ikEnableTable;
	};
	std::vector<VMDIKEnable> _ikEnableData;

	DWORD _startTime;

	uint32_t indicesnum;

private:

	ComPtr<ID3D12Resource> LoadTextureFromFile(string& texPath);

	ID3D12Resource* CreateWhiteTexture();
	ID3D12Resource* CreateBlackTexture();

	HRESULT CreateMaterial();

	HRESULT CreateBone(FILE* fp);

	//マテリアル及びテクスチャのビュー生成
	HRESULT CreateMaterialAndTextureView();

	HRESULT CreateTransform();

	HRESULT CreateDescHeap();

	HRESULT LoadPMDFile(const char* path);

	void recursiveMatrixMultiply(BoneNode* node, const XMMATRIX& mat);

	void MotionUpdate();

	float GetYFromXOn(float x, const XMFLOAT2& a, const XMFLOAT2& b, uint8_t n);

	//CCDIKによりボーン方向を解決
	void SolveCCOIK(const PMDIK& ik);
	//余弦定理IK
	void SolveConsineIK(const PMDIK& ik);
	//LookAt行列
	void SolveLookAt(const PMDIK& ik);
	//場合分け
	void IKSolve(uint32_t frameNo);

public:
	PMDmodel(/*Wrapper* _dx12, const char* filepath, PMDobject& object*/);
	~PMDmodel();

public:
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="_dx12">DirectX基本情報</param>
	/// <returns>成功：true / 失敗：false</returns>
	static bool StaticInitialize(Wrapper* _dx12);
	/// <summary>
	/// インスタンス生成及びPMDファイル読み込み
	/// </summary>
	/// <param name="filepath">ファイルパス</param>
	/// <returns>インスタンス</returns>
	static PMDmodel* CreateFromPMD(const char* filepath);

	bool Initialize(const char* filepath);

	void Update();
	void UpdateWorldMatrix();

	void Draw(ID3D12GraphicsCommandList* cmdList, bool isShadow = false);

	void playAnimation();

	HRESULT LoadVMDFile(const unsigned int Number, const char* path);

	//アクセッサ
	ID3D12DescriptorHeap* MaterialDescHeap() { return materialDescHeap.Get(); }
	ID3D12DescriptorHeap* DescHeap() { return descHeap.Get(); }
	D3D12_VERTEX_BUFFER_VIEW VbView() { return vbView; }
	D3D12_INDEX_BUFFER_VIEW IbView() { return ibView; }
	std::vector<Material> Materials() { return materials; }
	XMFLOAT3 GetBonePos(std::string KeyName) {
		PMDBone bone;
		BoneNode _bone;
		_bone = _boneNodeTable.at(KeyName);
		bone = pmdBones.at(94);
		return _bone.startPos;
		//return bone.pos;
	}
	std::vector<DirectX::XMMATRIX> GetBoneMat() { return boneMatrices; }
	XMMATRIX GetWorldMat() {
		// スケール、回転、平行移動行列の計算
		XMMATRIX matScale, matRot, matTrans;
		matScale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(XMConvertToRadians(this->rotation.z));
		matRot *= XMMatrixRotationX(XMConvertToRadians(this->rotation.x));
		matRot *= XMMatrixRotationY(XMConvertToRadians(this->rotation.y));
		matTrans = XMMatrixTranslation(this->position.x, this->position.y, this->position.z);

		// ワールド行列の合成
		matWorld = XMMatrixIdentity(); // 変形をリセット
		matWorld *= matScale; // ワールド行列にスケーリングを反映
		matWorld *= matRot; // ワールド行列に回転を反映
		matWorld *= matTrans; // ワールド行列に平行移動を反映

		return matWorld;
	}

	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}
	XMFLOAT3 GetPosition() {
		return position;
	}
	void SetMatRot(XMMATRIX rot) { this->matRot = rot; }

public://メンバ変数
	ComPtr<ID3D12Resource> PMDconstBuffB1; // 定数バッファ

	// 色
	XMFLOAT4 color = { 1,1,1,1 };
	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	// ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	// ローカルワールド変換行列
	XMMATRIX matWorld;

	XMMATRIX matRot = XMMatrixIdentity();;

	bool animation = false;
	//選択中のモーションデータ
	int vmdNumber = 0;
	int oldVmdNumber = 0;
	bool a = false;
};