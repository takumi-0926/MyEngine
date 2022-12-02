#pragma once
#include <json.hpp>
#include <string>
#include <vector>
#include <DirectXMath.h>

struct JsonData {
	struct ObjectData {
		std::string name;
		DirectX::XMVECTOR trans;
		DirectX::XMVECTOR scale;
		DirectX::XMVECTOR rot;
	};

	struct ColliderData {
		std::string name;
		DirectX::XMVECTOR trans;
		DirectX::XMVECTOR scale;
		DirectX::XMVECTOR rot;
	};

	struct spawnPointData {
		std::string name;
		DirectX::XMVECTOR trans;
		DirectX::XMVECTOR scale;
	};

	struct eventData {
		std::string name;
		DirectX::XMVECTOR trans;
		DirectX::XMVECTOR scale;
	};

	std::vector<ColliderData> colliders;
	std::vector<spawnPointData> spawnpoints;
	std::vector<eventData> events;
	std::vector<ObjectData> objects;
};

class JsonLoader {
	using string = std::string;

public:
	static const string baseDirectory;//ディレクトリファイル名
	static const string defaultTextureFileName;//デフォルトテクスチャファイル名
	static JsonData* jsonData;

	static JsonData* LoadJsonFile(const string& modelname);

	static JsonData* LoadRecursion(nlohmann::json deserialized);
};