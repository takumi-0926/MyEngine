#include "JsonLoader.h"
#include <fstream>

const std::string JsonLoader::baseDirectory = "Resources/stageData/";
const std::string JsonLoader::defaultTextureFileName = "";//デフォルトテクスチャファイル名
JsonData* JsonLoader::jsonData = new JsonData();

JsonData* JsonLoader::LoadJsonFile(const string& modelname)
{
	//フルパス取得
	const string fullPath = baseDirectory + modelname + ".json";

	std::ifstream file;

	//ファイルオープン
	file.open(fullPath);

	if (file.fail()) { assert(0); }

	nlohmann::json deserialized;

	file >> deserialized;

	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//再帰読み込み
	return LoadRecursion(deserialized);
}

JsonData* JsonLoader::LoadRecursion(nlohmann::json deserialized)
{
	string name = deserialized["name"].get<string>();

	assert(name.compare("scene") == 0);


	for (nlohmann::json& object : deserialized["objects"]) {

		assert(object.contains("type"));

		string type = object["type"].get<string>();

		//MESH
		if (type.compare("MESH") == 0) {
			jsonData->objects.emplace_back(JsonData::ObjectData{});

			//参照取得
			JsonData::ObjectData& objectData = jsonData->objects.back();

			if (object.contains("file_name")) {
				objectData.name = object["file_name"];
			}

			nlohmann::json& transform = object["transform"];
			//平行移動
			objectData.trans.m128_f32[0] = float(transform["translation"][1]);
			objectData.trans.m128_f32[1] = float(transform["translation"][2]);
			objectData.trans.m128_f32[2] = -float(transform["translation"][0]);
			objectData.trans.m128_f32[3] = 1.0f;
			//回転
			objectData.rot.m128_f32[0] = -float(transform["rotation"][1]);
			objectData.rot.m128_f32[1] = -float(transform["rotation"][2]);
			objectData.rot.m128_f32[2] = float(transform["rotation"][0]);
			objectData.rot.m128_f32[3] = 0.0f;
			//スケーリング
			objectData.scale.m128_f32[0] = float(transform["scaling"][1]);
			objectData.scale.m128_f32[1] = float(transform["scaling"][2]);
			objectData.scale.m128_f32[2] = float(transform["scaling"][0]);
			objectData.scale.m128_f32[3] = 0.0f;
		}

		//SPAWNPOINT
		if (type.compare("SPAWNPOINT") == 0) {
			jsonData->spawnpoints.emplace_back(JsonData::spawnPointData{});

			//参照取得
			JsonData::spawnPointData& eventData = jsonData->spawnpoints.back();

			if (object.contains("file_name")) {
				eventData.name = object["file_name"];
			}

			nlohmann::json& transform = object["file_name"];
			//平行移動
			eventData.trans.m128_f32[0] = float(transform["center"][1]);
			eventData.trans.m128_f32[1] = float(transform["center"][2]);
			eventData.trans.m128_f32[2] = -float(transform["center"][0]);
			eventData.trans.m128_f32[3] = 1.0f;
			//スケーリング
			eventData.scale.m128_f32[0] = float(transform["size"][1]);
			eventData.scale.m128_f32[1] = float(transform["size"][2]);
			eventData.scale.m128_f32[2] = float(transform["size"][0]);
			eventData.scale.m128_f32[3] = 0.0f;
		}

		//EVENT
		if (type.compare("EVENT") == 0) {
			jsonData->events.emplace_back(JsonData::eventData{});

			//参照取得
			JsonData::eventData& eventData = jsonData->events.back();

			if (object.contains("file_name")) {
				eventData.name = object["file_name"];
			}

			nlohmann::json& transform = object["file_name"];
			//平行移動
			eventData.trans.m128_f32[0] = float(transform["center"][1]);
			eventData.trans.m128_f32[1] = float(transform["center"][2]);
			eventData.trans.m128_f32[2] = -float(transform["center"][0]);
			eventData.trans.m128_f32[3] = 1.0f;
			//スケーリング
			eventData.scale.m128_f32[0] = float(transform["size"][1]);
			eventData.scale.m128_f32[1] = float(transform["size"][2]);
			eventData.scale.m128_f32[2] = float(transform["size"][0]);
			eventData.scale.m128_f32[3] = 0.0f;
		}

		//子供
		//if (object.contains("children")) {
		//	LoadRecursion(deserialized);
		//}
	}
	return jsonData;
}
