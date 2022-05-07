//#pragma once
//#include <DirectXMath.h>
//#include <string>
//#include <vector>
//
//struct IKLink {
//	int index;
//
//	bool existAngleLimited;
//
//	bool limitAngleMin;
//
//	bool limitAngleMax;
//};
//
//ウェイト
//struct Weight {
//	ウェイトタイプ
//	enum Type {
//		BDEF1,//ボーンのみ
//		BDEF2,//ボーン2つとボーン1のウェイト値（PMD方式）
//		BDEF4,//ボーン4つとそれぞれのウェイト値（ウェイト合計が1.0の保証なし）
//		SDEF,//BDEF2に加え、SDEF用のfloat3（Vector3）が3つ。本当は補正値が必要
//	};
//	Type type;
//
//	int born1;
//
//	int born2;
//
//	int born3;
//
//	int born4;
//
//	float weight1;
//
//	float weight2;
//
//	float weight3;
//
//	float weight4;
//
//	DirectX::XMFLOAT3 c;
//
//	DirectX::XMFLOAT3 r0;
//
//	DirectX::XMFLOAT3 r1;
//};
//
//ボーン
//struct Bone {
//	std::wstring Name;
//
//	std::string EngName;
//
//	DirectX::XMFLOAT3 position;
//
//	int parentIndex;
//
//	int transformationLevel;
//
//	unsigned short flag;
//
//	DirectX::XMFLOAT3 coordOffset;
//
//	int childrenIndex;
//
//	int impartParentIndex;
//
//	float impartRate;
//
//	DirectX::XMFLOAT3 fixedAxis;
//
//	DirectX::XMFLOAT3 localAxisX;
//
//	DirectX::XMFLOAT3 localAxisY;
//
//	DirectX::XMFLOAT3 localAxisZ;
//
//	int externalparnetKey;
//
//	int ikTargetIndex;
//
//	int ikLoopCount;
//
//	float ikUnitAngle;
//
//	std::vector<IKLink> ikLinks;
//};
//
//頂点情報
//struct Vertex {
//	位置
//	DirectX::XMFLOAT3 position;
//	法線
//	DirectX::XMFLOAT3 normal;
//	UV
//	DirectX::XMFLOAT2 uv;
//	追加UV
//	std::vector<DirectX::XMFLOAT4> additional_uv;
//	ウェイト
//	Weight weight;
//	エッジ倍率
//	float edge_diameter;
//	インデックス
//	unsigned short index;
//	マテリアル
//	Material materials;
//	ボーン
//	Bone bone;
//};
//
//マテリアル（材質）
//struct Material {
//	DirectX::XMFLOAT4 diffuse;
//
//	DirectX::XMFLOAT3 specular;
//
//	float specularStrength;
//
//	DirectX::XMFLOAT3 ambient;
//
//	int colorMaptextureIndex;
//
//	int toonTextureIndex;
//	材質ごとの頂点数
//	int vertexNum;
//};
//モデルデータ
//struct PMXModelData {
//	Vertex vertex;
//
//	std::vector<Vertex> vertices;
//	std::vector<unsigned short> indices;
//};
//
//PMX読み取り
//bool LoadPmx(PMXModelData& data, const std::wstring& _filePath);
