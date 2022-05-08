#include "gameManager.h"
#include "baseObject.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

bool GameManager::Initalize(Wrapper* dx12, Audio* audio, Input* input)
{
	// nullptrチェック
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

	//カメラをセット
	camera = new DebugCamera(Application::window_width, Application::window_height, input);
	BaseObject::SetCamera(camera);

	FbxObject3d::SetDevice(dx12->GetDevice());
	FbxObject3d::SetCamera(camera);
	FbxObject3d::CreateGraphicsPipeline();

	//基本オブジェクト--------------
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
	obj01->SetPosition({ 0,20,0 });

	pmdModel.reset(new PMDmodel(dx12->GetDevice(),"Resources/Model/初音ミクmetal.pmd", *pmdObject));
	pmdObject.reset(new PMDobject());

	//MMDオブジェクト----------------
	//pModel = PMDmodel::Create();
	////pModel->CreateModel("Resources/獅白ぼたん/PMX/獅白ぼたん.pmd");
	//pModel->CreateModel("Resources/Model/初音ミクmetal.pmd");

	//pmdObj = PMDobject::Create();
	//pmdObj->SetModel(pModel);
	//pmdObj->Update();
	//pmdObj->scale = { 0.1,0.1,0.1 };
	//pmdObj->SetPosition({ 0,20,-1 });

	//FBXオブジェクト----------------
	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("cube");
	fbxObj1 = new FbxObject3d;
	fbxObj1->Initialize();
	fbxObj1->Setmodel(fbxModel1);

	//スプライト---------------------
	sprite01 = Sprite::Create(0, { 0.0f,0.0f,0.0f });
	sprite02 = Sprite::Create(1, { 0.0f,0.0f,0.0f });
	sprite03 = Sprite::Create(2, { 0.0f,0.0f,0.0f });

	FbxLoader::GetInstance()->LoadModelFromFile("cube");

	camera->SetTarget({ 0,21,0 });
	camera->SetDistance(3.0f);

	input->Update();
	//audio->Load();

	SceneNum = 3;
	move.flag = false;

	return true;
}

void GameManager::Update()
{
	camera->Update();
	obj01->Update();
	//pmdObj->Update();
	fbxObj1->Update();

	//放物線運動
	{
		if (input->Trigger(DIK_1) && move.flag != true) {
			move.moveNum = PARABOLA;
			move.flag = true;
			move.time = 0.0f;
			move.v = 0.0f;
		}

		if (move.flag == true && move.moveNum == PARABOLA) {
			move.v = move.gravity * move.time;
			obj01->position.z += 5.0f;
			obj01->position.y += move.v0 - move.v;
			pmdObj->position.z += 5.0f;
			pmdObj->position.y += move.v0 - move.v;

			move.time += (1.0f / 120.0f);
			if (move.time >= 1.0f) {
				move.flag = false;
				move.moveNum = NONE;
				obj01->position.z = 0.0f;
				obj01->position.y = 20.0f;
				pmdObj->position.y = 20.0f;
				pmdObj->position.z = 0.0f;
			}
		}
	}

	//空気抵抗
	{
		if (input->Trigger(DIK_2) && move.flag != true) {
			move.moveNum = AIR;
			move.flag = true;
			move.time = 0.0f;
			move.v = 0.0f;
		}

		if (move.flag == true && move.moveNum == AIR) {
			move.v = move.gravity * move.time;
			obj01->position.y += move.vy - move.v + move.air_resister;
			pmdObj->position.y += move.vy - move.v + move.air_resister;

			if (move.time >= 0.3f) {
				move.air_resister = move.k * move.v;
			}
			move.time += (1.0f / 60.0f);
			if (move.time >= 2.0f) {
				move.flag = false;
				move.moveNum = NONE;
				move.air_resister = 0.0f;
				obj01->position.y = 20.0f;
				pmdObj->position.y = 20.0f;
			}
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

	//タイトル
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
	//ゲーム
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
		//レイと平面の当たり判定
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
	//エンド
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
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	Sprite::PreDraw(cmdList);

	sprite02->Draw();

	Sprite::PostDraw();

	//深度バッファクリア
	dx12->ClearDepthBuffer();

	BaseObject::PreDraw(cmdList);

	obj01->Draw();

	//pmdObj->Draw();
	pmdObject->Draw();
	pmdModel->Draw(cmdList);

	BaseObject::PostDraw();

	fbxObj1->Draw(cmdList);

	if (SceneNum == TITLE) {
		Sprite::PreDraw(cmdList);

		sprite01->Draw();

		Sprite::PostDraw();
		//Sprite::PreDraw(cmdList);

		//sprite02->Draw();

		//Sprite::PostDraw();
		//// 深度バッファクリア
		//dx12->ClearDepthBuffer();

		BaseObject::PreDraw(cmdList);

		obj01->Draw();

		BaseObject::PostDraw();

	}
	else if (SceneNum == GAME) {

		//Sprite::PreDraw(cmdList);

		//sprite02->Draw();

		//Sprite::PostDraw();
		// 深度バッファクリア
		dx12->ClearDepthBuffer();

		//オブジェクトの描画
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

