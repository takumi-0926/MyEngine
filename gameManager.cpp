#include "gameManager.h"
#include "baseObject.h"
#include "FbxLoader.h"

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

bool GameManager::Initalize(Wrapper* dx12, Audio* audio, Input* input)
{
	// nullptr�`�F�b�N
	assert(dx12);
	assert(input);
	assert(audio);

	this->dx12 = dx12;
	this->input = input;
	this->audio = audio;

	//this->audio->Load();

	if (!Sprite::loadTexture(0, L"Resources/Title.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(1, L"Resources/haikei.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(2, L"Resources/end.png")) {
		assert(0);
		return false;
	}

	//�J�������Z�b�g
	camera = new DebugCamera(Application::window_width, Application::window_height, input);
	BaseObject::SetCamera(camera);

	model01 = Model::Create();
	model01->CretaeFromObj("Block");
	model02 = Model::Create();
	model02->CretaeFromObj("combere");
	model03 = Model::Create();
	model03->CretaeFromObj("untitled");

	obj01 = Object3Ds::Create();
	obj01->SetModel(model01);
	obj01->Update();
	obj01->scale = { 1,1,1 };
	obj01->SetPosition({ 0,0,0 });

	pModel = PMDmodel::Create();
	pModel->CreateModel("Resources/Model/�����~�Nmetal.pmd");

	pmdObj = PMDobject::Create();
	pmdObj->SetModel(pModel);
	pmdObj->Update();
	pmdObj->scale = { 0.1,0.1,0.1 };
	pmdObj->SetPosition({ 0,0,-1 });

	sprite01 = Sprite::Create(0, { 0.0f,0.0f,0.0f });
	sprite02 = Sprite::Create(1, { 0.0f,0.0f,0.0f });
	sprite03 = Sprite::Create(2, { 0.0f,0.0f,0.0f });

	FbxLoader::GetInstance()->LoadModelFromFile("cube");

	camera->SetTarget({ 0,1,0 });
	camera->SetDistance(3.0f);

	input->Update();
	//audio->Load();

	SceneNum = 3;

	return true;
}

void GameManager::Update()
{
	camera->Update();
	obj01->Update();
	pmdObj->Update();

	if (input->Trigger(DIK_SPACE) && move.flag != true) {
		move.flag = true;
		move.time = 0.0f;
		move.v = 0.0f;
	}

	if (move.flag == true) {
		move.v = move.gravity * move.time;
		obj01->position.z += 5.0f;
		obj01->position.y += move.v0 - move.v;
		pmdObj->position.z += 5.0f;
		pmdObj->position.y += move.v0 - move.v;

		move.time += (1.0f /  60.0f);
		if (move.time >= 1.0f) {
			move.flag = false;
			obj01->position.z = 0.0f;
			obj01->position.y = 0.0f;
			pmdObj->position.y = 0.0f;
			pmdObj->position.z = 0.0f;
		}
	}

	if (input->Push(DIK_A)) {
		obj01->position.x -= 1.0f;
	}
	if (input->Push(DIK_D)) {
		obj01->position.x += 1.0f;
	}
	if (input->Push(DIK_W)) {
		obj01->position.z += 1.0f;
	}
	if (input->Push(DIK_S)) {
		obj01->position.z -= 1.0f;
	}

	if (input->Push(DIK_RIGHT)) {
		//audio->Play(0);
		obj01->rotation.x += 1.0f;
	}
	if (input->Push(DIK_UP)) {
		obj01->rotation.y += 1.0f;
	}

	//�^�C�g��
	if (SceneNum == TITLE) {

		if (resetFlag == false) {
			Initalize(dx12, audio, input);
			resetFlag = true;
		}
		if (input->Trigger(DIK_SPACE)) {
			SceneNum = GAME;
			sprite01->Update();
		}
		if (input->Push(DIK_A)) {
			obj01->position.x -= 0.01f;
		}
		if (input->Push(DIK_D)) {
			obj01->position.x += 0.01f;
		}
		if (input->Push(DIK_W)) {
			obj01->position.z += 0.01f;
		}
		if (input->Push(DIK_S)) {
			obj01->position.z -= 0.01f;
		}

		if (input->Push(DIK_RIGHT)) {
			obj01->rotation.x += 1.0f;
		}
		if (input->Push(DIK_UP)) {
			obj01->rotation.y += 1.0f;
		}
		obj01->Update();

	}
	//�Q�[��
	else if (SceneNum == GAME) {

		camera->Update();
		sprite01->Update();
		obj01->Update();

		for (int i = 0; i < 25; i++)
		{
			obj02[i]->Update();
		}

		for (int i = 0; i < 10; i++)
		{
			if (shotFlag[i] == false) {
				obj03[i]->position = obj01->position;
				obj03[i]->position.x = obj01->position.x + 0.34f;
			}
			else if (shotFlag[i] == true) {
				obj03[i]->position.z += 0.1f;
			}

			sqhere[i].center =
				XMVectorSet(
					obj03[i]->position.x,
					obj03[i]->position.y,
					obj03[i]->position.z, 1);

			obj03[i]->Update();
		}

		for (int i = 0; i < 25; i++) {
			triangle[i].p0 = XMVectorSet(
				obj02[i]->position.x - 0.41f,
				obj02[i]->position.y - 0.41f,
				obj02[i]->position.z, 1);
			triangle[i].p1 = XMVectorSet(
				obj02[i]->position.x + 0.41f,
				obj02[i]->position.y - 0.41f,
				obj02[i]->position.z, 1);
			triangle[i].p2 = XMVectorSet(
				obj02[i]->position.x - 0.41f,
				obj02[i]->position.y + 0.41f,
				obj02[i]->position.z, 1);
			triangle[i].normal = XMVectorSet(0.0f, 0.0f, -1.0f, 0);
		}

		if (input->Push(DIK_A)) {
			obj01->position.x -= 0.01f;
		}
		if (input->Push(DIK_D)) {
			obj01->position.x += 0.01f;
		}
		if (input->Push(DIK_W)) {
			obj01->position.y += 0.01f;
		}
		if (input->Push(DIK_S)) {
			obj01->position.y -= 0.01f;
		}

		if (input->Trigger(DIK_SPACE)) {
			for (int i = 0; i < 10; i++)
			{
				if (shotFlag[i] == false) {
					shotFlag[i] = true;
					break;
				}
			}
		}

		ray.start = XMVectorSet(input->GetPos().x / 1000.0f, input->GetPos().y / 1000.0f, 0, 1);
		if (ray.start.m128_f32[0] != 0) {
			ray.start.m128_f32[0] = input->GetPos().x;
		}
		//���C�ƕ��ʂ̓����蔻��
		{
			XMVECTOR interR;
			float distance = 0;
			for (int i = 0; i < 25; i++)
			{
				for (int j = 0; j < 10; j++)
				{

					bool hit = Coliision::CheckSqhere2Triangle(
						sqhere[j], triangle[i], &interR);

					if (hit) {
						if (cubeFlag[i] == false) {
							count -= 1;
						}
						shotFlag[j] = false;
						cubeFlag[i] = true;
					}
				}
			}
		}

		if (count <= 0) {
			SceneNum = END;
		}
	}
	//�G���h
	else if (SceneNum == END) {
		if (input->Trigger(DIK_SPACE)) {
			sprite03->Update();
			SceneNum = TITLE;
			resetFlag = false;
		}
	}
}

void GameManager::Draw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	Sprite::PreDraw(cmdList);

	sprite02->Draw();

	Sprite::PostDraw();

	//�[�x�o�b�t�@�N���A
	dx12->ClearDepthBuffer();

	BaseObject::PreDraw(cmdList);

	obj01->Draw();

	pmdObj->Draw();

	BaseObject::PostDraw();

	if (SceneNum == TITLE) {
		Sprite::PreDraw(cmdList);

		sprite01->Draw();

		Sprite::PostDraw();
		//Sprite::PreDraw(cmdList);

		//sprite02->Draw();

		//Sprite::PostDraw();
		//// �[�x�o�b�t�@�N���A
		//dx12->ClearDepthBuffer();

		BaseObject::PreDraw(cmdList);

		obj01->Draw();

		BaseObject::PostDraw();

	}
	else if (SceneNum == GAME) {

		//Sprite::PreDraw(cmdList);

		//sprite02->Draw();

		//Sprite::PostDraw();
		// �[�x�o�b�t�@�N���A
		dx12->ClearDepthBuffer();

		//�I�u�W�F�N�g�̕`��
		BaseObject::PreDraw(cmdList);
		obj01->Draw();

		for (int i = 0; i < 24; i++) {
			if (cubeFlag[i] == false) {
				obj02[i]->Draw();
			}
		}

		for (int i = 0; i < 10; i++) {
			if (shotFlag[i] == true) {
				obj03[i]->Draw();
			}
		}

		BaseObject::PostDraw();
	}
	else if (SceneNum == END) {
		Sprite::PreDraw(cmdList);

		sprite03->Draw();

		Sprite::PostDraw();
	}
}

