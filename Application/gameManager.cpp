#include <algorithm>
#include "gameManager.h"
#include "..\object\baseObject.h"
#include "..\FBX\FbxLoader.h"
#include "..\FBX\FbxObject3d.h"
#include "../PMD/PMXLoader.h"

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
	if (!Sprite::loadTexture(3, L"Resources/circle_white.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(4, L"Resources/circle_red.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(5, L"Resources/line_white.png")) {
		assert(0);
		return false;
	}

	//�J�������Z�b�g
	camera = new DebugCamera(Application::window_width, Application::window_height, input);
	mainCamera = new Camera(Application::window_width, Application::window_height);
	camera->SetTarget({ 0,0,0 });
	camera->SetDistance(20.0f);
	Wrapper::SetCamera(camera);
	dx12->SceneUpdate();
	camera->Update();

	//���C�g�Z�b�g
	light = light->Create();
	light->SetLightColor({ 1,1,1 });
	Wrapper::SetLight(light);

	//��{�I�u�W�F�N�g--------------
	model01 = Model::CreateFromOBJ("Block");
	model02 = Model::CreateFromOBJ("ground");
	model03 = Model::CreateFromOBJ("togetoge");
	model04 = Model::CreateFromOBJ("Gate");
	model05 = Model::CreateFromOBJ("sqhere");
	model06 = Model::CreateFromOBJ("maru");

	obj01 = Object3Ds::Create();
	obj01->SetModel(model01);
	obj01->Update();
	obj01->scale = { 1,1,1 };
	obj01->SetPosition({ -10,20,0 });

	//obj02 = Object3Ds::Create();
	//obj02->SetModel(model02);
	//obj02->Update();
	//obj02->scale = { 2,2,2 };
	//obj02->rotation.y = 90;
	//obj02->SetPosition({ 0,-30,0 });

	stage = Stage::Create();
	stage->SetModel(model02);
	stage->Update();
	stage->scale = { 2,2,2 };
	stage->rotation.y = 90;
	stage->SetPosition({ 0,-30,0 });

	obj03 = Object3Ds::Create();
	obj03->SetModel(model04);
	obj03->Update();
	obj03->scale = { 50,50,50 };
	obj03->SetPosition({ 0,0,150 });

	for (int i = 0; i < 3; i++)
	{
		float rX = rand() % 200 - 100;
		float rZ = rand() % 200 - 100;
		obj04[i] = Object3Ds::Create();
		obj04[i]->SetModel(model03);
		obj04[i]->Update();
		obj04[i]->scale = { 10,10,10 };
		obj04[i]->SetPosition({ rX,40,rZ });
	}
	obj04[0]->SetPosition({ 100,0,100 });
	obj04[1]->SetPosition({ -100,0,100 });
	obj04[2]->SetPosition({ 100,0,-100 });


	//MMD�I�u�W�F�N�g----------------
	pmdObject.reset(new PMDobject(dx12));
	pmdModel.reset(new PMDmodel(dx12, "Resources/Model/�����~�Nmetal.pmd", *pmdObject));
	pmdModel->scale = { 1,1,1 };
	pmdModel->SetPosition({ 0,0,0 });

	//PMXModelData pmxData{};
	//LoadPmx(pmxData,L"Resources\\�����ڂ���\\�����ڂ���.pmx");

	//FBX�I�u�W�F�N�g----------------
	FbxObject3d::SetDevice(dx12->GetDevice());
	FbxObject3d::SetCamera(mainCamera);
	FbxObject3d::CreateGraphicsPipeline();

	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	fbxObj1 = new FbxObject3d;
	fbxObj1->Initialize();
	fbxObj1->Setmodel(fbxModel1);
	fbxObj1->SetPosition({ -10,29,0 });

	//�X�v���C�g---------------------
	sprite01 = Sprite::Create(0, { 0.0f,0.0f });
	sprite02 = Sprite::Create(1, { 0.0f,0.0f });
	sprite03 = Sprite::Create(2, { 0.0f,0.0f });
	sprite04 = Sprite::Create(3, { 0.0f,0.0f });
	sprite05 = Sprite::Create(4, { 0.0f,0.0f });
	sprite06 = Sprite::Create(5, { 500.0f,0.0f });

	HitBox::CreatePipeline(dx12);
	HitBox::CreateTransform();
	HitBox::CreateHitBox(pmdModel->GetBonePos("����"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("�E��"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("����"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("�E�Ђ�"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("���Ђ�"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("�E�Ђ�"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("���Ђ�"), model01);

	//�e�X�g�v���C
	for (int i = 0; i < NUM_OBJ; i++) {
		test[i] = Obj::Create();
		test[i]->SetModel(model01);

		line[i] = object2d::Create();
	}
	test[0]->stat = 2;	//�ŏ��̂P�ڂ͐Î~
	test[0]->position = pmdModel->position;
	test[0]->position.y = pmdModel->position.y + 50;
	test[0]->pos.y = test[0]->position.y;
	input->Update();
	//audio->Load();

	//_test = ShaderTest::Create();
	//_test->SetModel(model05);

	SceneNum = 3;
	move1.flag = false;
	move2.flag = false;

	fbxObj1->PlayAnimation();
	pmdModel->playAnimation();
	pmdModel->animation = true;

	return true;
}

void GameManager::Update()
{
	//imgui
	//static bool blnChk = false;
	//static int radio = 0;
	//static float eneSpeed = 0.0f;
	//{
	//	ImGui::Begin("Rendering Test Menu");
	//	ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);
	//	imgui��UI�R���g���[��
	//	ImGui::Checkbox("EnemyPop", &blnChk);
	//	ImGui::Checkbox("test", &pmdModel->a);
	//	ImGui::RadioButton("Debug Camera", &radio, 0);
	//	ImGui::SameLine();
	//	ImGui::RadioButton("Game Camera", &radio, 1);
	//	ImGui::SameLine();
	//	ImGui::RadioButton("Radio 3", &radio, 2);
	//	int nSlider = 0;
	//	ImGui::SliderFloat("Enemy Speed", &eneSpeed, 0.05f, 1.0f);
	//	static float fSlider = 0.0f;
	//	ImGui::SliderFloat("Float Slider", &fSlider, 0.0f, 100.0f);
	//	static float col3[3] = {};
	//	ImGui::ColorPicker3("ColorPicker3", col3, ImGuiColorEditFlags_::ImGuiColorEditFlags_InputRGB);
	//	static float col4[4] = {};
	//	ImGui::ColorPicker4("ColorPicker4", col4, ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
	//	ImGui::End();
	//	�J�����؂�ւ�
	//	static bool isCamera = false;
	//	if (radio == 0 && isCamera == false) {
	//		Wrapper::SetCamera(camera);
	//		isCamera = true;
	//	}
	//	else if (radio == 1 && isCamera == true) {
	//		Wrapper::SetCamera(mainCamera);
	//		isCamera = false;
	//	}
	//}

	if (input->Trigger(DIK_M)) {
		pmdModel->playAnimation();
	}
	//�G�l�~�[�̐���
	//if (blnChk == true) {
	//	Enemy* ene = Enemy::Create();
	//	int r = rand() % 10;
	//	if (r % 2 == 1) {
	//		ene->SetModel(model03);
	//		ene->mode = 1;
	//	}
	//	else if (r % 2 != 1) {
	//		ene->SetModel(model05);
	//		ene->mode = 2;
	//	}
	//	if (r == 5 || r == 6) {
	//		ene->SetModel(model06);
	//		ene->mode = 3;
	//	}
	//	ene->scale = { 10,10,10 };
	//	ene->step = eneSpeed / 10000;
	//	ene->alive = true;
	//	_enemy.push_back(ene);

	//	Sqhere _sqhere;
	//	_sqhere.radius = 5.0f;
	//	_sqhere.center = XMVectorSet(ene->position.x, ene->position.y, ene->position.z, 1);
	//	sqhere.push_back(_sqhere);

	//	blnChk = false;
	//}
	//�G�l�~�[�֌W�̐���
	for (int i = 0; i < _enemy.size(); i++) {
		_enemy[i]->moveUpdate(pmdModel->position, obj04, obj03->position);
		_enemy[i]->rotation.y += 1.0f;
		sqhere[i].center = XMVectorSet(_enemy[i]->position.x, _enemy[i]->position.y, _enemy[i]->position.z, 1);
		if (_enemy[i]->alive == true) { continue; }
		_enemy.erase(_enemy.begin());
		sqhere.erase(sqhere.begin());

	}

	//�����蔻��i�v���C���[ / �G / �ŏI�֖�j
	for (int i = 0; i < sqhere.size(); i++) {
		for (int j = 0; j < HitBox::_hit.size(); j++) {
			bool Hhit = Coliision::CheckSqhere2Sqhere(sqhere[i], HitBox::_hit[j]);
			XMVECTOR inter;
			bool Ghit = Coliision::CheckSqhere2Triangle(sqhere[i], triangle[0], &inter);

			if (Ghit == true && reception <= 0) {
				Hhit = false;
				gateHP -= 1;
				reception = 600;
			}
			if (Hhit == true) {
				_enemy[i]->alive = false;
			}
			if (gateHP <= 0) { SceneNum = END; }
			reception--;
		}
	}

	triangle[0].p0 = XMVectorSet(obj03->position.x - 100.0, obj03->position.y, obj03->position.z, 1);
	triangle[0].p1 = XMVectorSet(obj03->position.x - 100.0, obj03->position.y + 120.0, obj03->position.z, 1);
	triangle[0].p2 = XMVectorSet(obj03->position.x + 100.0, obj03->position.y, obj03->position.z, 1);
	triangle[0].normal = XMVectorSet(0.0f, 0.0f, 1.0f, 0);

	//MT4
	{
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
			//if (input->Trigger(DIK_3)) {
			//	sqhere[0].center = XMVECTOR{ obj01->position.x,obj01->position.y,obj01->position.z ,1 };
			//	sqhere[1].center = XMVECTOR{ obj02->position.x,obj02->position.y,obj02->position.z ,1 };

			//	//�~�Ɖ~
			//	bool hit = Coliision::CheckSqhere2Sqhere(sqhere[0], sqhere[1]);

			//	//�^���ʌv�Z
			//	move1.p = move1.m * move1.vx;
			//	move2.p = move2.m * move2.vx;

			//	if (hit) {
			//		move1.flag = false;
			//		move2.flag = true;
			//		move1.vx = 0;
			//	}

			//	if (move1.flag == true) {
			//		move1.vx += move1.accel;
			//	}
			//	if (move2.flag == true) {
			//		move2.vx = (move1.p / move2.m);
			//		move2.flag = false;
			//	}
			//	obj02->position.x += move2.vx;
			//	obj01->position.x += move1.vx;
			//}
			////���Z�b�g
			//if (input->Trigger(DIK_R)) {
			//	move1.flag = true;
			//	move2.flag = false;
			//	move1.vx = 0;
			//	move2.vx = 0;
			//	obj01->position.x = -10;
			//	obj02->position.x = 10;
			//}
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

		//�Փ�
		if (sprite06->Pos().x - 256 <= sprite04->Pos().x &&
			sprite06->Pos().y <= sprite04->Pos().y + 256 &&
			sprite06->Pos().x >= sprite04->Pos().x - 256 &&
			sprite06->Pos().y >= sprite04->Pos().y) {
			colFlag = true;
		}
		else {
			colFlag = false;
		}

		if (input->Trigger(DIK_RETURN)) {
			easing.fadeFlag = true;
		}
		if (easing.fadeFlag == true) {
			easing.addTime2 += 1.0f / 60.0f;
			easing.timeRate2 = easing.addTime2 / easing.maxTime;
			if (easing.addTime2 / easing.maxTime >= 1.0f) { easing.timeRate2 = 1.0f; }
			XMFLOAT3 pos = obj01->position;
			pos.x = easing.easeIn(easing.end, easing.end2, easing.timeRate2);
			obj01->position = pos;
			//�J�ڏI��
			if (pos.x == easing.end2) {
				easing.fadeFlag = false;
				easing.FadeReset();
			}
		}
	}

	// ���̒ǉ�?
	//if (input->Trigger(DIK_UP))
	//{
	//	if (_idx_obj < NUM_OBJ)
	//	{
	//		_idx_obj++;
	//		test[_idx_obj]->stat = 1;
	//		test[_idx_obj]->vel.clear(0);
	//		test[_idx_obj]->pos = test[_idx_obj - 1]->pos;
	//		// �����̐eobj���P���obj��
	//		test[_idx_obj]->link0 = test[_idx_obj - 1];
	//		// �P���obj�̎qobj��������
	//		test[_idx_obj - 1]->link1 = test[_idx_obj];

	//		line[_idx_obj - 1]->CreateLine(test[_idx_obj]->position.x, test[_idx_obj]->position.y, test[_idx_obj - 1]->position.x, test[_idx_obj - 1]->position.y);
	//	}
	//}

	//���C�g
	{
		static XMVECTOR lightDir = { 0,1,5,0 };

		if (input->Push(DIK_T)) { lightDir.m128_f32[1] += 1.0f; }
		if (input->Push(DIK_G)) { lightDir.m128_f32[1] -= 1.0f; }
		if (input->Push(DIK_H)) { lightDir.m128_f32[0] += 1.0f; }
		if (input->Push(DIK_F)) { lightDir.m128_f32[0] -= 1.0f; }

		light->SetLightDir(lightDir);
	}
	//�ړ�
	{
		pmdModel->oldVmdNumber = pmdModel->vmdNumber;
		if (directInput->leftStickX() < 0.0f || directInput->leftStickX() > 0.0f || directInput->leftStickY() < 0.0f || directInput->leftStickY() > 0.0f) {
			pmdModel->vmdNumber = vmdData::WALK;
			if (directInput->getTriggerZ() != 0) {
				speed = 2.0f;
			}
			else { speed = 1.0f; }
			if (input->Push(DIK_A) || directInput->leftStickX() < 0.0f) {
				pmdModel->position = MoveLeft(pmdModel->position);
				pmdModel->rotation.y = angleHorizonal + (90 * directInput->getLeftX() / 1);
				for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveLeft(HitBox::GetHit()[i]->position); }
			}
			if (input->Push(DIK_D) || directInput->leftStickX() > 0.0f) {
				pmdModel->position = MoveRight(pmdModel->position);
				pmdModel->rotation.y = angleHorizonal - (90 * directInput->getLeftX() / 1);
				for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveRight(HitBox::GetHit()[i]->position); }
			}
			if (input->Push(DIK_W) || directInput->leftStickY() < 0.0f) {
				pmdModel->position = MoveBefore(pmdModel->position);
				pmdModel->rotation.y = angleHorizonal - (90 * directInput->getLeftY() / 1);
				for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveBefore(HitBox::GetHit()[i]->position); }
			}
			if (input->Push(DIK_S) || directInput->leftStickY() > 0.0f) {
				pmdModel->position = MoveAfter(pmdModel->position);
				pmdModel->rotation.y = angleHorizonal + (90 * directInput->getLeftY() / 1);
				for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveAfter(HitBox::GetHit()[i]->position); }
			}
			float a = directInput->getLeftX();
			float b = directInput->getLeftY();
			if (a + b > 0) {
				pmdModel->rotation.y = angleHorizonal + 180 * sin(a + b);
			}
			else {
				pmdModel->rotation.y = angleHorizonal + 180 * sin(a + b);
			}
		}
		else {
			pmdModel->vmdNumber = vmdData::WAIT;
		}

		if (input->Push(DIK_RIGHT)) {
			//audio->Play(0);
			obj01->rotation.x += 1.0f;
			test[0]->position.x += 1.0f;
		}
		if (input->Push(DIK_UP)) {
			obj01->rotation.y += 1.0f;
		}

		if (input->Push(DIK_2)) {
			test[0]->position.x += 1.0f;
		}
		if (input->Push(DIK_1)) {
			test[0]->position.x -= 1.0f;
		}
		if (input->Push(DIK_3)) {
			test[0]->position.y -= 1.0f;
		}
		if (input->Push(DIK_4)) {
			test[0]->position.y += 1.0f;
		}

	}

	//�J�������[�N(�v���C���[)
	float angleH = 100.0f;
	float angleV = 40.0f;
	if (directInput->rightStickX() >= 0.5f || directInput->rightStickX() <= -0.5f) {
		angleHorizonal +=
			XMConvertToRadians(angleH * directInput->getRightX());
	}
	if (directInput->rightStickY() >= 0.5f || directInput->rightStickY() <= -0.5f) {
		angleVertical +=
			XMConvertToRadians(angleV * directInput->getRightY());
		//�����p�x
		if (angleVertical >= 20) {
			angleVertical = 20;
		}
		//�����p�x
		if (angleVertical <= -20) {
			angleVertical = -20;
		}
	}
	if (input->Push(DIK_RIGHT) || input->Push(DIK_LEFT) || input->Push(DIK_UP) || input->Push(DIK_DOWN)) {
		//�E
		if (input->Push(DIK_RIGHT)) {
			angleHorizonal +=
				XMConvertToRadians(angleH * directInput->getRightX());
		}
		//��
		if (input->Push(DIK_LEFT)) {
			angleHorizonal -=
				XMConvertToRadians(angleH);
		}
		//��
		if (input->Push(DIK_UP)) {
			angleVertical -=
				XMConvertToRadians(angleV);
			//�����p�x
			if (angleVertical <= -20) {
				angleVertical = -20;
			}
		}
		//��
		if (input->Push(DIK_DOWN)) {
			angleVertical +=
				XMConvertToRadians(angleV);
			//�����p�x
			if (angleVertical >= 20) {
				angleVertical = 20;
			}
		}
	}

	//�J�������[�N(�Ǐ])
	{
		//�J�����ƃv���C���[�̋���������
		const float distanceFromPlayerToCamera = 30.0f;

		//static float CAME_HEIGHT = 10.0f;
		//�J�����̍���
		const float cameraHeight = 30.0f;

		//�J�����̐��ʃx�N�g��������
		vv0 = { 0.0f,0.0f,1.0f,0.0f };

		//�c���̉�]
		rotM = XMMatrixRotationX(XMConvertToRadians(angleVertical));
		vv0 = XMVector3TransformNormal(vv0, rotM);

		//�����̉�]
		rotM = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));
		vv0 = XMVector3TransformNormal(vv0, rotM);

		//
		XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

		//�J�����ʒu������
		XMFLOAT3 _eye;
		_eye.x = pmdModel->position.x + direction.x * distanceFromPlayerToCamera;
		_eye.y = pmdModel->position.y + direction.y * distanceFromPlayerToCamera;
		_eye.z = pmdModel->position.z + direction.z * distanceFromPlayerToCamera;
		_eye.y += cameraHeight;
		//camera->SetEye(_eye);
		mainCamera->SetEye(_eye);

		//�J���������_������
		XMFLOAT3 _target;
		_target.x = pmdModel->position.x - direction.x * distanceFromPlayerToCamera;
		_target.y = pmdModel->position.y - direction.y * distanceFromPlayerToCamera;
		_target.z = pmdModel->position.z - direction.z * distanceFromPlayerToCamera;
		_target.y += cameraHeight / 3;
		//camera->SetTarget(_target);
		mainCamera->SetTarget(_target);

		//��]
		//pmdModel->rotation.y = angleHorizonal;

		//camera->Update();
		mainCamera->Update();
	}

	obj01->position = pmdModel->position;
	//�X�V����()
	{
		dx12->SceneUpdate();
		camera->Update();
		obj01->Update();
		stage->Update();
		//obj02->Update();
		obj03->Update();
		fbxObj1->Update();
		pmdModel->Update();
		sprite06->Update();
		for (int i = 0; i < 3; i++) {
			obj04[i]->Update();
		}
		for (int i = 0; i < NUM_OBJ; i++) {
			test[i]->Update();
		}

		HitBox::mainUpdate(pmdModel->GetBoneMat(),pmdModel->rotation);
		light->Update();
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

	//sprite02->Draw();
	//sprite01->Draw();

	Sprite::PostDraw();

	//�[�x�o�b�t�@�N���A
	dx12->ClearDepthBuffer();

	BaseObject::PreDraw(cmdList);
	obj01->Draw();
	//obj02->Draw();
	stage->Draw();
	obj03->Draw();
	for (int i = 0; i < 3; i++) {
		obj04[i]->Draw();
	}
	for (int i = 0; i < _enemy.size(); i++) {
		_enemy[i]->Draw();
	}
	HitBox::mainDraw();
	pmdObject->Draw();
	dx12->SceneDraw();
	pmdModel->Draw(cmdList);

	for (int i = 0; i < NUM_OBJ; i++) {
		test[i]->Draw();
	}

	BaseObject::PostDraw();

	//fbxObj1->Draw(cmdList);

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
		// �R�}���h���X�g�̎擾
		ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

		Sprite::PreDraw(cmdList);

		sprite02->Draw();

		Sprite::PostDraw();
	}
}
