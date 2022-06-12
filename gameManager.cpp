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
	camera->SetTarget({ 7,20,0 });
	camera->SetDistance(20.0f);
	camera->Update();

	//��{�I�u�W�F�N�g--------------
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
	obj01->SetPosition({ -10,20,0 });

	obj02 = Object3Ds::Create();
	obj02->SetModel(model01);
	obj02->Update();
	obj02->scale = { 1,1,1 };
	obj02->SetPosition({ 10,20,0 });

	pmdObject.reset(new PMDobject());
	pmdModel.reset(new PMDmodel(dx12->GetDevice(), "Resources/Model/�����~�Nmetal.pmd", *pmdObject));
	pmdModel->scale = { 1,1,1 };
	pmdModel->SetPosition({ 0,20,0 });

	//MMD�I�u�W�F�N�g----------------
	//pModel = PMDmodel::Create();
	////pModel->CreateModel("Resources/�����ڂ���/PMX/�����ڂ���.pmd");
	//pModel->CreateModel("Resources/Model/�����~�Nmetal.pmd");

	//pmdObj = PMDobject::Create();
	//pmdObj->SetModel(pModel);
	//pmdObj->Update();

	//FBX�I�u�W�F�N�g----------------
	FbxObject3d::SetDevice(dx12->GetDevice());
	FbxObject3d::SetCamera(camera);
	FbxObject3d::CreateGraphicsPipeline();

	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	fbxObj1 = new FbxObject3d;
	fbxObj1->Initialize();
	fbxObj1->Setmodel(fbxModel1);

	//�X�v���C�g---------------------
	sprite01 = Sprite::Create(0, { 0.0f,0.0f,0.0f });
	sprite02 = Sprite::Create(1, { 0.0f,0.0f,0.0f });
	sprite03 = Sprite::Create(2, { 0.0f,0.0f,0.0f });

	//FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	input->Update();
	//audio->Load();

	SceneNum = 3;
	move1.flag = false;
	move2.flag = false;

	fbxObj1->PlayAnimation();

	return true;
}

void GameManager::Update()
{
	camera->Update();
	obj01->Update();
	obj02->Update();
	fbxObj1->Update();
	//pmdModel->Update();
	//pmdObject->Update();


	//�������^��
	{
		if (input->Trigger(DIK_1) && move1.flag != true) {
			move1.moveNum = PARABOLA;
			move1.flag = true;
			move1.time = 0.0f;
			move1.v = 0.0f;
		}

		if (move1.flag == true && move1.moveNum == PARABOLA) {
			move1.v = move1.gravity * move1.time;
			obj01->position.z += 5.0f;
			obj01->position.y += move1.v0 - move1.v;
			pmdModel->position.z += 5.0f;
			pmdModel->position.y += move1.v0 - move1.v;

			move1.time += (1.0f / 120.0f);
			if (move1.time >= 1.0f) {
				move1.flag = false;
				move1.moveNum = NONE;
				obj01->position.z = 0.0f;
				obj01->position.y = 20.0f;
				pmdModel->position.y = 20.0f;
				pmdModel->position.z = 0.0f;
			}
		}
	}

	//��C��R
	{
		if (input->Trigger(DIK_2) && move1.flag != true) {
			move1.moveNum = AIR;
			move1.flag = true;
			move1.time = 0.0f;
			move1.v = 0.0f;
		}

		if (move1.flag == true && move1.moveNum == AIR) {
			move1.v = move1.gravity * move1.time;
			obj01->position.y += move1.vy - move1.v + move1.air_resister;
			pmdModel->position.y += move1.vy - move1.v + move1.air_resister;

			if (move1.time >= 0.3f) {
				move1.air_resister = move1.k * move1.v;
			}
			move1.time += (1.0f / 60.0f);
			if (move1.time >= 2.0f) {
				move1.flag = false;
				move1.moveNum = NONE;
				move1.air_resister = 0.0f;
				obj01->position.y = 20.0f;
				pmdModel->position.y = 20.0f;
			}
		}
	}

	//�Փˁi�^���ʕۑ����j
	{
		if (input->Trigger(DIK_3)) {
			sqhere[0].center = XMVECTOR{ obj01->position.x,obj01->position.y,obj01->position.z ,1 };
			sqhere[1].center = XMVECTOR{ obj02->position.x,obj02->position.y,obj02->position.z ,1 };

			//�~�Ɖ~
			bool hit = Coliision::CheckSqhere2Sqhere(sqhere[0], sqhere[1]);

			//�^���ʌv�Z
			move1.p = move1.m * move1.vx;
			move2.p = move2.m * move2.vx;

			if (hit) {
				move1.flag = false;
				move2.flag = true;
				move1.vx = 0;
			}

			if (move1.flag == true) {
				move1.vx += move1.accel;
			}
			if (move2.flag == true) {
				move2.vx = (move1.p / move2.m);
				move2.flag = false;
			}
			obj02->position.x += move2.vx;
			obj01->position.x += move1.vx;
		}
		//���Z�b�g
		if (input->Trigger(DIK_R)) {
			move1.flag = true;
			move2.flag = false;
			move1.vx = 0;
			move2.vx = 0;
			obj01->position.x = -10;
			obj02->position.x = 10;
		}
	}

	//���˕Ԃ�
	{
		if (input->Trigger(DIK_4))
		{
			obj01->position.y = 30;
			obj01->position.x = -10;
			move1.v = 0;
			move1.v0 = 0;
			move1.time = 0;
			move1.flag = true;
		}

		if (move1.flag == true) {
			move1.v = move1.v0 + (move1.gravity * move1.time);
			obj01->position.y += -move1.v;
			obj01->position.x += 0.2f;
			move1.time += 1 / 120.0f;

			if (obj01->position.y <= 10) {
				move1.v0 = -(move1.v * move1.e);
				move1.time = 0;
			}
		}
	}

	//�~�^��
	{
		if (input->Push(DIK_5)) {


			if (true) {

				angle += 2.0f;
				XMVECTOR hf = { 1,0,0,0 };
				rot = XMMatrixRotationZ(XMConvertToRadians(angle));
				hf = XMVector3TransformNormal(hf, rot);

				XMFLOAT3 direction = { hf.m128_f32[0],hf.m128_f32[1],hf.m128_f32[2] };
				obj01->position.x += direction.x;
				obj01->position.y += direction.y;
				obj01->position.z += direction.z;
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

		//camera->Update();
		//sprite01->Update();
		//obj01->Update();

		//for (int i = 0; i < 25; i++)
		//{
		//	obj02[i]->Update();
		//}

		//for (int i = 0; i < 10; i++)
		//{
		//	if (shotFlag[i] == false) {
		//		obj03[i]->position = obj01->position;
		//		obj03[i]->position.x = obj01->position.x + 0.34f;
		//	}
		//	else if (shotFlag[i] == true) {
		//		obj03[i]->position.z += 0.1f;
		//	}

		//	sqhere[i].center =
		//		XMVectorSet(
		//			obj03[i]->position.x,
		//			obj03[i]->position.y,
		//			obj03[i]->position.z, 1);

		//	obj03[i]->Update();
		//}

		//for (int i = 0; i < 25; i++) {
		//	triangle[i].p0 = XMVectorSet(
		//		obj02[i]->position.x - 0.41f,
		//		obj02[i]->position.y - 0.41f,
		//		obj02[i]->position.z, 1);
		//	triangle[i].p1 = XMVectorSet(
		//		obj02[i]->position.x + 0.41f,
		//		obj02[i]->position.y - 0.41f,
		//		obj02[i]->position.z, 1);
		//	triangle[i].p2 = XMVectorSet(
		//		obj02[i]->position.x - 0.41f,
		//		obj02[i]->position.y + 0.41f,
		//		obj02[i]->position.z, 1);
		//	triangle[i].normal = XMVectorSet(0.0f, 0.0f, -1.0f, 0);
		//}

		//if (input->Push(DIK_A)) {
		//	obj01->position.x -= 0.01f;
		//}
		//if (input->Push(DIK_D)) {
		//	obj01->position.x += 0.01f;
		//}
		//if (input->Push(DIK_W)) {
		//	obj01->position.y += 0.01f;
		//}
		//if (input->Push(DIK_S)) {
		//	obj01->position.y -= 0.01f;
		//}

		//if (input->Trigger(DIK_SPACE)) {
		//	for (int i = 0; i < 10; i++)
		//	{
		//		if (shotFlag[i] == false) {
		//			shotFlag[i] = true;
		//			break;
		//		}
		//	}
		//}

		//ray.start = XMVectorSet(input->GetPos().x / 1000.0f, input->GetPos().y / 1000.0f, 0, 1);
		//if (ray.start.m128_f32[0] != 0) {
		//	ray.start.m128_f32[0] = input->GetPos().x;
		//}
		////���C�ƕ��ʂ̓����蔻��
		//{
		//	XMVECTOR interR;
		//	float distance = 0;
		//	for (int i = 0; i < 25; i++)
		//	{
		//		for (int j = 0; j < 10; j++)
		//		{

		//			bool hit = Coliision::CheckSqhere2Triangle(
		//				sqhere[j], triangle[i], &interR);

		//			if (hit) {
		//				if (cubeFlag[i] == false) {
		//					count -= 1;
		//				}
		//				shotFlag[j] = false;
		//				cubeFlag[i] = true;
		//			}
		//		}
		//	}
		//}

		//if (count <= 0) {
		//	SceneNum = END;
		//}
	}
	//�G���h
	else if (SceneNum == END) {
		//if (input->Trigger(DIK_SPACE)) {
		//	sprite03->Update();
		//	SceneNum = TITLE;
		//	resetFlag = false;
		//}
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
	obj02->Draw();

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

		//for (int i = 0; i < 24; i++) {
		//	if (cubeFlag[i] == false) {
		//		obj02[i]->Draw();
		//	}
		//}

		//for (int i = 0; i < 10; i++) {
		//	if (shotFlag[i] == true) {
		//		obj03[i]->Draw();
		//	}
		//}

		BaseObject::PostDraw();
	}
	else if (SceneNum == END) {
		Sprite::PreDraw(cmdList);

		sprite03->Draw();

		Sprite::PostDraw();
	}
}

