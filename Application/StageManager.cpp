#include "StageManager.h"
#include "object/Model.h"
#include "object/object3D.h"

StageManager* StageManager::GetInstance()
{
	static StageManager* instance = new StageManager();
	return instance;
}

void StageManager::Initialize()
{
	//�X�J�C�h�[��-------------------
	skyDomeModel = Model::CreateFromOBJ("skydome");
	skyDome = Object3Ds::Create(skyDomeModel);
	skyDome->scale = { 4,4,4 };
	skyDome->position = { 0,350,0 };

	//�X�e�[�W���f��
	stageModels.insert(std::make_pair("Ground", Model::CreateFromOBJ("Ground")));
	stageModels.insert(std::make_pair("Gate", Model::CreateFromOBJ("Gate")));
	stageModels.insert(std::make_pair("Wall", Model::CreateFromOBJ("Wall")));
	stageModels.insert(std::make_pair("Tree", Model::CreateFromOBJ("Tree")));
	stageModels.insert(std::make_pair("Cliff", Model::CreateFromOBJ("Cliff")));
	stageModels.insert(std::make_pair("Foundation", Model::CreateFromOBJ("Foundation")));
	stageModels.insert(std::make_pair("Spike", Model::CreateFromOBJ("spike")));
	stageModels.insert(std::make_pair("WallRock", Model::CreateFromOBJ("WallRock")));
	stageModels.insert(std::make_pair("Camp", Model::CreateFromOBJ("Camp")));
	stageModels.insert(std::make_pair("Tent", Model::CreateFromOBJ("Tent")));

	//�X�e�[�W�f�[�^�y�у��f���f�[�^�ǂݍ���
	stageData = JsonLoader::LoadJsonFile("titleStageData");
	for (auto& objectData : stageData->objects) {
		Model* model = nullptr;
		decltype(stageModels)::iterator it = stageModels.find(objectData.name);
		if (it != stageModels.end()) { model = it->second; }
		Stage* newObject = Stage::Create(model);

		//�����蔻������邩�ǂ���
		if (objectData.name != "Tree" && objectData.name != "Cliff") {
			newObject->SetCollision();
		}
		//���O��t����
		if (objectData.name == "Foundation") { newObject->SetObjectNum(ObjectType::FounDation); }
		if (objectData.name == "Gate") { newObject->SetObjectNum(ObjectType::Gate); }

		XMFLOAT3 pos{};
		XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		XMFLOAT3 rot{};
		XMStoreFloat3(&rot, objectData.rot);
		newObject->rotation = rot;

		XMFLOAT3 scale{};
		XMStoreFloat3(&scale, objectData.scale);
		newObject->scale = scale;

		titleStages.push_back(newObject);
	}

	//��x���Z�b�g�i�X�e�[�W�̓ǂݍ��݂̂��߁j
	stageData->objects = {};
	stageData = JsonLoader::LoadJsonFile("stageData");
	for (auto& objectData : stageData->objects) {
		Model* model = nullptr;
		decltype(stageModels)::iterator it = stageModels.find(objectData.name);
		if (it != stageModels.end()) { model = it->second; }

		Stage* newObject = Stage::Create(model);

		XMFLOAT3 pos{};
		XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		XMFLOAT3 rot{};
		XMStoreFloat3(&rot, objectData.rot);
		newObject->rotation = rot;

		XMFLOAT3 scale{};
		XMStoreFloat3(&scale, objectData.scale);
		newObject->scale = scale;

		if (objectData.name == "Foundation") {
			newObject->SetObjectNum(ObjectType::FounDation);
		}
		if (objectData.name == "Gate") {
			newObject->SetObjectNum(ObjectType::Gate);
		}

		stages.push_back(newObject);
	}

	//��x���Z�b�g�i�x�[�X�L�����v�̓ǂݍ��݂̂��߁j
	stageData->objects = {};
	stageData = JsonLoader::LoadJsonFile("BaseCampData");
	for (auto& objectData : stageData->objects) {
		Model* model = nullptr;
		decltype(stageModels)::iterator it = stageModels.find(objectData.name);
		if (it != stageModels.end()) { model = it->second; }

		Stage* newObject = Stage::Create(model);

		XMFLOAT3 pos{};
		XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		XMFLOAT3 rot{};
		XMStoreFloat3(&rot, objectData.rot);
		newObject->rotation = rot;

		XMFLOAT3 scale{};
		XMStoreFloat3(&scale, objectData.scale);
		newObject->scale = scale;

		baseCamp.push_back(newObject);
	}
}

void StageManager::Update()
{
	//�X�e�[�W�`��X�V����
	//�^�C�g���X�e�[�W
	for (auto& object : titleStages) {
		object->SetColliderInvisible(false);
		object->Update();
	}
	for (auto& object : stages) {
		if (UseStage == GameLocation::BaseCamp) { object->SetColliderInvisible(false); }
		if (UseStage == GameLocation::BaseStage) {
			object->SetColliderInvisible(true);
			object->Update();
		}
	}
	for (auto& object : baseCamp) {
		if (UseStage == GameLocation::BaseStage) { object->SetColliderInvisible(false); }
		if (UseStage == GameLocation::BaseCamp) {
			object->SetColliderInvisible(true);
			object->Update();
		}
	}
	skyDome->Update();
}

void StageManager::Draw()
{
	skyDome->Draw();
	//�^�C�g���X�e�[�W
	if (UseStage == 0) {
		for (auto& object : titleStages) {
			object->Draw();
		}
	}
	//�x�[�X�L�����v
	else if (UseStage == 1) {
		for (auto& object : baseCamp) {
			object->Draw();
		}
	}
	//�ʏ�X�e�[�W
	else if (UseStage == 2) {
		for (auto& object : stages) {
			object->Draw();
		}
	}
}

void StageManager::ShadowDraw()
{
	//�^�C�g���X�e�[�W
	if (UseStage == 0) {
		for (auto& object : titleStages) {
			object->ShadowDraw();
		}
	}
	//�x�[�X�L�����v
	else if (UseStage == 1) {
		for (auto& object : baseCamp) {
			object->ShadowDraw();
		}
	}
	//�ʏ�X�e�[�W
	else if (UseStage == 2) {
		for (auto& object : stages) {
			object->ShadowDraw();
		}
	}
}
