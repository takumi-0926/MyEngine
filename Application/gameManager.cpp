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

	//カメラをセット
	camera = new DebugCamera(Application::window_width, Application::window_height, input);
	mainCamera = new Camera(Application::window_width, Application::window_height);
	camera->SetTarget({ 0,0,0 });
	camera->SetDistance(20.0f);
	Wrapper::SetCamera(mainCamera);
	dx12->SceneUpdate();
	camera->Update();

	//ライトセット
	light = light->Create();
	light->SetLightColor({ 1,1,1 });
	Wrapper::SetLight(light);

	//基本オブジェクト--------------
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

	for (int i = 0; i < 6; i++)
	{
		cannon[i] = DefCannon::Create(model01);
		cannon[i]->SetModel(model03);
		cannon[i]->Update();
		cannon[i]->scale = { 10,10,10 };
	}
	cannon[0]->SetPosition({ -50,0,  0 });
	cannon[1]->SetPosition({  50,0,  0 });
	cannon[2]->SetPosition({  50,0, 50 });
	cannon[3]->SetPosition({ -50,0, 50 });
	cannon[4]->SetPosition({  50,0,100 });
	cannon[5]->SetPosition({ -50,0,100 });

	//MMDオブジェクト----------------
	pmdObject.reset(new PMDobject(dx12));
	pmdModel.reset(new PMDmodel(dx12, "Resources/Model/初音ミクmetal.pmd", *pmdObject));
	pmdModel->scale = { 1,1,1 };
	pmdModel->SetPosition({ 0,10,0 });

	//PMXModelData pmxData{};
	//LoadPmx(pmxData,L"Resources\\獅白ぼたん\\獅白ぼたん.pmx");

	//FBXオブジェクト----------------
	FbxObject3d::SetDevice(dx12->GetDevice());
	FbxObject3d::SetCamera(mainCamera);
	FbxObject3d::CreateGraphicsPipeline();

	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	fbxObj1 = new FbxObject3d;
	fbxObj1->Initialize();
	fbxObj1->Setmodel(fbxModel1);
	fbxObj1->SetPosition({ -10,29,0 });

	//スプライト---------------------
	sprite01 = Sprite::Create(0, { 0.0f,0.0f });
	sprite02 = Sprite::Create(1, { 0.0f,0.0f });
	sprite03 = Sprite::Create(2, { 0.0f,0.0f });
	sprite04 = Sprite::Create(3, { 0.0f,0.0f });
	sprite05 = Sprite::Create(4, { 0.0f,0.0f });
	sprite06 = Sprite::Create(5, { 500.0f,0.0f });

	for (int i = 0; i < P_HP; i++) {
		static float xpos = 5.0f;
		hp[i] = Sprite::Create(3, { xpos * i,10.0f });
		hp[i]->SetSize(XMFLOAT2(20, 15));
		hp[i]->Update();
	}

	//ヒットボックス-----------------
	HitBox::CreatePipeline(dx12);
	HitBox::CreateTransform();
	HitBox::CreateHitBox(pmdModel->GetBonePos("頭先"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("右肩"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("左肩"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("右ひじ"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("左ひじ"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("右ひざ"), model01);
	HitBox::CreateHitBox(pmdModel->GetBonePos("左ひざ"), model01);

	triangle[0].p0 = XMVectorSet(obj03->position.x - 100.0, obj03->position.y, obj03->position.z, 1);
	triangle[0].p1 = XMVectorSet(obj03->position.x - 100.0, obj03->position.y + 120.0, obj03->position.z, 1);
	triangle[0].p2 = XMVectorSet(obj03->position.x + 100.0, obj03->position.y, obj03->position.z, 1);
	triangle[0].normal = XMVectorSet(0.0f, 0.0f, 1.0f, 0);

	//テストプレイ
	for (int i = 0; i < NUM_OBJ; i++) {
		test[i] = Obj::Create();
		test[i]->SetModel(model01);

		line[i] = object2d::Create();
	}
	test[0]->stat = 2;	//最初の１個目は静止
	test[0]->position = pmdModel->position;
	test[0]->position.y = pmdModel->position.y + 50;
	test[0]->pos.y = test[0]->position.y;
	input->Update();
	audio->Load();

	SceneNum = TITLE;
	move1.flag = false;
	move2.flag = false;

	fbxObj1->PlayAnimation();
	pmdModel->playAnimation();
	pmdModel->animation = true;

	return true;
}

void GameManager::Update()
{
	if (input->Trigger(DIK_M)) {
		pmdModel->playAnimation();
	}
	//imgui
	static bool blnChk = false;
	static int radio = 0;
	static float eneSpeed = 0.0f;
	{
		ImGui::Begin("Rendering Test Menu");
		ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);
		//imguiのUIコントロール
		ImGui::Text("test %.2f ,%.2f ,%.2f ,%.2f", a, b, c, d);
		ImGui::Checkbox("EnemyPop", &blnChk);
		ImGui::Checkbox("test", &pmdModel->a);
		ImGui::RadioButton("Debug Camera", &radio, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Game Camera", &radio, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Radio 3", &radio, 2);
		int nSlider = 0;
		ImGui::SliderFloat("Enemy Speed", &eneSpeed, 0.05f, 1.0f);
		static float col3[3] = {};
		ImGui::ColorPicker3("ColorPicker3", col3, ImGuiColorEditFlags_::ImGuiColorEditFlags_InputRGB);
		static float col4[4] = {};
		ImGui::ColorPicker4("ColorPicker4", col4, ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
		ImGui::End();

		//カメラ切り替え
		static bool isCamera = false;
		if (radio == 0 && isCamera == false) {
			Wrapper::SetCamera(camera);
			isCamera = true;
		}
		else if (radio == 1 && isCamera == true) {
			Wrapper::SetCamera(mainCamera);
			isCamera = false;
		}
	}

	//MT4
	{
		//放物線運動
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

		//空気抵抗
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

		//衝突（運動量保存則）
		{
			//if (input->Trigger(DIK_3)) {
			//	sqhere[0].center = XMVECTOR{ obj01->position.x,obj01->position.y,obj01->position.z ,1 };
			//	sqhere[1].center = XMVECTOR{ obj02->position.x,obj02->position.y,obj02->position.z ,1 };

			//	//円と円
			//	bool hit = Coliision::CheckSqhere2Sqhere(sqhere[0], sqhere[1]);

			//	//運動量計算
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
			////リセット
			//if (input->Trigger(DIK_R)) {
			//	move1.flag = true;
			//	move2.flag = false;
			//	move1.vx = 0;
			//	move2.vx = 0;
			//	obj01->position.x = -10;
			//	obj02->position.x = 10;
			//}
		}

		//跳ね返り
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

		//円運動
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

		//衝突
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
			//遷移終了
			if (pos.x == easing.end2) {
				easing.fadeFlag = false;
				easing.FadeReset();
			}
		}
		// 物体追加?
//if (input->Trigger(DIK_UP))
//{
//	if (_idx_obj < NUM_OBJ)
//	{
//		_idx_obj++;
//		test[_idx_obj]->stat = 1;
//		test[_idx_obj]->vel.clear(0);
//		test[_idx_obj]->pos = test[_idx_obj - 1]->pos;
//		// 自分の親objを１つ上のobjに
//		test[_idx_obj]->link0 = test[_idx_obj - 1];
//		// １つ上にobjの子objを自分に
//		test[_idx_obj - 1]->link1 = test[_idx_obj];

//		line[_idx_obj - 1]->CreateLine(test[_idx_obj]->position.x, test[_idx_obj]->position.y, test[_idx_obj - 1]->position.x, test[_idx_obj - 1]->position.y);
//	}
//}

	}

	//ライト
	{
		static XMVECTOR lightDir = { 0,1,5,0 };

		if (input->Push(DIK_T)) { lightDir.m128_f32[1] += 1.0f; }
		if (input->Push(DIK_G)) { lightDir.m128_f32[1] -= 1.0f; }
		if (input->Push(DIK_H)) { lightDir.m128_f32[0] += 1.0f; }
		if (input->Push(DIK_F)) { lightDir.m128_f32[0] -= 1.0f; }

		light->SetLightDir(lightDir);
	}

	//タイトル
	if (SceneNum == TITLE) {

		if (resetFlag == false) {
			//Initalize(dx12, audio, input);
			resetFlag = true;
		}
		if (input->Trigger(DIK_SPACE) || directInput->IsButtonUp(directInput->DownButton)) {
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
		//エネミーの生成
		{
			static float popTime = 0;
			if (popTime >= 10.0f) {
				Enemy* ene = Enemy::Create();
				int r = rand() % 10;
				if (r % 2 == 1) {
					ene->SetModel(model03);
					ene->mode = 1;
				}
				else if (r % 2 != 1) {
					ene->SetModel(model05);
					ene->mode = 2;
				}
				if (r == 5 || r == 6) {
					ene->SetModel(model06);
					ene->mode = 3;
				}
				ene->scale = { 10,10,10 };
				ene->step = 0.00005;
				ene->alive = true;
				_enemy.push_back(ene);

				Sqhere _sqhere;
				_sqhere.radius = 5.0f;
				_sqhere.center = XMVectorSet(ene->position.x, ene->position.y, ene->position.z, 1);
				sqhere.push_back(_sqhere);

				popTime = 0;
			}
			else {
				popTime += 1.0f / 60.0f;
			}
		}
		//エネミー関係の制御
		for (int i = 0; i < _enemy.size(); i++) {
			_enemy[i]->moveUpdate(pmdModel->position, cannon, obj03->position);
			_enemy[i]->rotation.y += 1.0f;
			sqhere[i].center = XMVectorSet(_enemy[i]->position.x, _enemy[i]->position.y, _enemy[i]->position.z, 1);
			if (_enemy[i]->alive == true) { continue; }
			_enemy.erase(_enemy.begin());
			sqhere.erase(sqhere.begin());
		}
		//当たり判定（プレイヤー / 敵 / 最終関門）
		for (int i = 0; i < sqhere.size(); i++) {
			for (int j = 0; j < HitBox::_hit.size(); j++) {
				bool Hhit = Coliision::CheckSqhere2Sqhere(sqhere[i], HitBox::_hit[j]);
				XMVECTOR inter;
				bool Ghit = Coliision::CheckSqhere2Triangle(sqhere[i], triangle[0], &inter);

				//ゲート攻撃
				if (Ghit == true && reception <= 0 && _enemy[i]->attackHit == true) {
					if (_enemy[i]->mode != 3) { continue; }
					Hhit = false;
					_enemy[i]->attackHit = false;
					gateHP -= 1;
					reception = 600;
				}

				//エネミーダメージ
				if (Hhit == true && _enemy[i]->attackHit == true) {
					if (_enemy[i]->mode != 2) { continue; }
					_enemy[i]->attackHit = false;
					_enemy[i]->damage = true;
					_enemy[i]->status.HP -= 1;
					playerHp -= 10;
					//体力がなくなっていれば
					if (_enemy[i]->status.HP >= 0) { continue; }
					_enemy[i]->alive = false;
				}
				//ゲームオーバー条件				
				if (gateHP <= 0 || playerHp <= 0) { SceneNum = END; }
				reception--;
			}
		}
		//当たり判定(プレイヤー/ステージ)仮
		if (pmdModel->position.y < stage->position.y + 40) {
			pmdModel->position.y = stage->position.y + 40;
		}
		//エネミー同士の当たり判定
		for (int i = 0; i < sqhere.size(); i++) {
			for (int j = 0; j < sqhere.size(); j++) {
				if (i == j) { continue; }
				bool Hhit = Coliision::CheckSqhere2Sqhere(sqhere[i], sqhere[j]);
				if (Hhit == true) {
					_enemy[i]->position = _enemy[i]->oldPos;
				}
			}
		}
		//移動
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
					for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveLeft(HitBox::GetHit()[i]->position); }
				}
				if (input->Push(DIK_D) || directInput->leftStickX() > 0.0f) {
					pmdModel->position = MoveRight(pmdModel->position);
					for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveRight(HitBox::GetHit()[i]->position); }
				}
				if (input->Push(DIK_W) || directInput->leftStickY() < 0.0f) {
					pmdModel->position = MoveBefore(pmdModel->position);
					for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveBefore(HitBox::GetHit()[i]->position); }
				}
				if (input->Push(DIK_S) || directInput->leftStickY() > 0.0f) {
					pmdModel->position = MoveAfter(pmdModel->position);
					for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveAfter(HitBox::GetHit()[i]->position); }
				}
				XMVECTOR v = { (directInput->getLeftX()),0.0f,-(directInput->getLeftY()),0.0f };
				XMMATRIX matRot = XMMatrixIdentity();
				//角度回転
				matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));
				v = XMVector3TransformNormal(v, matRot);
				XMFLOAT3 _v(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
				pmdModel->SetMatRot(LookAtRotation(_v, XMFLOAT3(0.0f, 1.0f, 0.0f)));

				//XMVECTOR q;
				//XMVECTOR normal;
				//XMVECTOR YAxis = { 0,1,0 };
				//normal = XMVector3Cross(v, YAxis);
				//normal = XMVector3Normalize(normal);
				//q.m128_f32[0] = q.m128_f32[1] = q.m128_f32[2] = 0.0f;
				//q.m128_f32[3] = 1.0f;
				//q = XMQuaternionRotationAxis(YAxis, angleHorizonal);
				//pmdModel->SetMatRot(XMMatrixRotationQuaternion(q));
			}
			else if (directInput->IsButtonUp(directInput->Button01) || input->Push(DIK_X)) {
				pmdModel->vmdNumber = vmdData::ATTACK;
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
		//カメラワーク(プレイヤー)
		float angleH = 100.0f;
		float angleV = 40.0f;
		if (directInput->rightStickX() >= 0.5f || directInput->rightStickX() <= -0.5f) {
			angleHorizonal +=
				XMConvertToRadians(angleH * directInput->getRightX());
		}
		if (directInput->rightStickY() >= 0.5f || directInput->rightStickY() <= -0.5f) {
			angleVertical +=
				XMConvertToRadians(angleV * directInput->getRightY());
			//制限角度
			if (angleVertical >= 20) {
				angleVertical = 20;
			}
			//制限角度
			if (angleVertical <= -20) {
				angleVertical = -20;
			}
		}
		if (input->Push(DIK_RIGHT) || input->Push(DIK_LEFT) || input->Push(DIK_UP) || input->Push(DIK_DOWN)) {
			//右
			if (input->Push(DIK_RIGHT)) {
				angleHorizonal +=
					XMConvertToRadians(angleH * directInput->getRightX());
			}
			//左
			if (input->Push(DIK_LEFT)) {
				angleHorizonal -=
					XMConvertToRadians(angleH);
			}
			//上
			if (input->Push(DIK_UP)) {
				angleVertical -=
					XMConvertToRadians(angleV);
				//制限角度
				if (angleVertical <= -20) {
					angleVertical = -20;
				}
			}
			//下
			if (input->Push(DIK_DOWN)) {
				angleVertical +=
					XMConvertToRadians(angleV);
				//制限角度
				if (angleVertical >= 20) {
					angleVertical = 20;
				}
			}
		}
		//カメラワーク(追従)
		{
			//カメラとプレイヤーの距離を決定
			const float distanceFromPlayerToCamera = 40.0f;

			//カメラの高さ
			const float cameraHeight = 30.0f;

			//カメラの正面ベクトルを決定
			vv0 = { 0.0f,0.0f,1.0f,0.0f };

			//縦軸の回転
			rotM = XMMatrixRotationX(XMConvertToRadians(angleVertical));
			vv0 = XMVector3TransformNormal(vv0, rotM);

			//横軸の回転
			rotM = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));
			vv0 = XMVector3TransformNormal(vv0, rotM);

			//
			XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

			//カメラ位置を決定
			XMFLOAT3 _eye;
			_eye.x = pmdModel->position.x + direction.x * distanceFromPlayerToCamera;
			_eye.y = pmdModel->position.y + direction.y * distanceFromPlayerToCamera;
			_eye.z = pmdModel->position.z + direction.z * distanceFromPlayerToCamera;
			_eye.y += cameraHeight;
			mainCamera->SetEye(_eye);

			//カメラ注視点を決定
			XMFLOAT3 _target;
			_target.x = pmdModel->position.x - direction.x * distanceFromPlayerToCamera;
			_target.y = pmdModel->position.y - direction.y * distanceFromPlayerToCamera;
			_target.z = pmdModel->position.z - direction.z * distanceFromPlayerToCamera;
			_target.y += cameraHeight / 3;
			mainCamera->SetTarget(_target);

			mainCamera->Update();
		}
		//更新処理(ゲーム)
		{
			static float gravity = 0.098f;
			static float v = 0.0f;
			v += gravity;
			pmdModel->position.y -= gravity;
		}
		//更新処理(固有)
		{
			dx12->SceneUpdate();
			camera->Update();
			obj01->Update();
			stage->Update();
			obj03->Update();
			fbxObj1->Update();
			pmdModel->Update();
			sprite06->Update();
			for (int i = 0; i < 6; i++) {
				cannon[i]->moveUpdate(_enemy);
				cannon[i]->Update();
			}
			for (int i = 0; i < NUM_OBJ; i++) {
				test[i]->Update();
			}

			HitBox::mainUpdate(pmdModel->GetBoneMat(), pmdModel->rotation);
			light->Update();
		}
	}
	//エンド
	else if (SceneNum == END) {
	}

	//SceneEffectManager::Update();

}

void GameManager::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	//fbxObj1->Draw(cmdList);

	if (SceneNum == TITLE) {
		Sprite::PreDraw(cmdList);

		sprite01->Draw();

		Sprite::PostDraw();

		BaseObject::PreDraw(cmdList);

		obj01->Draw();

		BaseObject::PostDraw();
	}
	else if (SceneNum == GAME) {
		Sprite::PreDraw(cmdList);

		//sprite02->Draw();
		//sprite01->Draw();

		Sprite::PostDraw();

		//深度バッファクリア
		dx12->ClearDepthBuffer();

		BaseObject::PreDraw(cmdList);
		obj01->Draw();
		stage->Draw();
		obj03->Draw();

		for (int i = 0; i < 6; i++) {
			cannon[i]->Draw();
		}

		for (int i = 0; i < _enemy.size(); i++) {
			if (_enemy[i]->damage == true){
				_enemy[i]->damegeCount += 1.0f / 60.0f;
				if (_enemy[i]->damegeCount >= 0.5f) {
					_enemy[i]->damage = false;
					_enemy[i]->damegeCount = 0;
				}
				continue;
			}

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

		Sprite::PreDraw(cmdList);

		for (int i = 0; i < playerHp; i++) {
			hp[i]->Draw();
		}

		Sprite::PostDraw();
	}
	else if (SceneNum == END) {
		// コマンドリストの取得
		ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

		Sprite::PreDraw(cmdList);

		sprite02->Draw();

		Sprite::PostDraw();
	}
}
