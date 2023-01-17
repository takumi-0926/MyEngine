#include "DefCannon.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

DefCannon::DefCannon()
{
}
DefCannon* DefCannon::Create(int _mode, Model* _model)
{
	// 3Dオブジェクトのインスタンスを生成
	DefCannon* instance = new DefCannon();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->position.y = 10;
	instance->shotVec = { 0,0,1,0 };
	// 初期化
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
		return nullptr;
	}

	if (_model) {
		instance->SetModel(_model);
	}

	instance->SetMode(_mode);
	return instance;
}
void DefCannon::BulletCreate(Model* _model)
{
	for (int i = 0; i < 10; i++)
	{
		Bullet* _bullet;
		_bullet = _bullet->Create(_model);
		_bullet->scale = { 1,1,1 };
		_bullet->SetAlive(false);
		_bullet->SetSpeed(4.0f);
		bullet.push_back(_bullet);
	}
}
void DefCannon::Update() {

	count += 1.0f / 60.0f;

	for (int i = 0; i < bullet.size(); i++) {
		if (bullet[i]->GetStatus().isAlive == false) {
			bullet[i]->SetBasePos(XMFLOAT3(position.x, position.y + 2.0f, position.z));
		}

		if (count >= bullet[i]->attackCount) {
			//一番初め(使われていなければ優先的に使う)
			if (i == 0 && bullet[i]->GetStatus().isAlive == false) {
				bullet[i]->SetAlive(true);
				bullet[i]->Update();
				count = 0;//カウントをゼロに
				continue;
			}
			if (i != 0) {
				//ひとつ前の弾が撃たれていないとき
				if (bullet[i - 1]->GetStatus().isAlive == false) {
					bullet[i]->Update();
					continue;
				}
				//弾以前の弾が全て生きていて自分が生きていない
				else if (bullet[i]->GetStatus().isAlive == false) {
					bullet[i]->SetAlive(true);
					count = 0;//カウントをゼロに
				}
			}
		}
		bullet[i]->Update();
	}

	Object3Ds::Update();
}
void DefCannon::Draw()
{
	Object3Ds::Draw();

	for (int i = 0; i < bullet.size(); i++) {
		if (!bullet[i]->GetStatus().isAlive) { continue; }
		bullet[i]->Draw();
	}
}

float DefCannon::distance(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	float distance;
	float x = abs(pos1.x - pos2.x);
	float z = abs(pos1.z - pos2.z);
	distance = std::sqrt(x * 2 + z * 2);
	return distance;
}
XMVECTOR DefCannon::objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	XMVECTOR distance;
	float x = pos1.x - pos2.x;
	float y = pos1.y - pos2.y;
	float z = pos1.z - pos2.z;
	distance = { x,y,z,0 };
	return distance;
}
XMVECTOR DefCannon::Normalize(XMVECTOR vec)
{
	float t; //ベクトルの大きさ
	float x = vec.m128_f32[0] * vec.m128_f32[0];//x成分
	float y = vec.m128_f32[1] * vec.m128_f32[1];//y成分
	float z = vec.m128_f32[2] * vec.m128_f32[2];//z成分
	//ベクトルの大きさを計算
	t = sqrt(x * x + y * y + z * z);
	//正規化
	XMVECTOR _vec;
	_vec.m128_f32[0] = vec.m128_f32[0] / t;
	_vec.m128_f32[1] = vec.m128_f32[1] / t;
	_vec.m128_f32[2] = vec.m128_f32[2] / t;

	return _vec;
}
XMFLOAT3 DefCannon::VectorToXMFloat(XMVECTOR vec)
{
	XMFLOAT3 ret;
	ret.x = vec.m128_f32[0];
	ret.y = vec.m128_f32[1];
	ret.z = vec.m128_f32[2];
	return ret;
}

void DefCannon::moveUpdate(std::vector<Enemy*> ePos)
{
	AutoShot(ePos);

	FixedShot();

	//matRot = LookAtRotation(VectorToXMFloat(Normalize(objectVector(ePos[No]->GetPosition(), position))), XMFLOAT3(0, 1, 0));
}

DefCannon* DefCannon::Appearance(int type, Model* hand, Model* assault, Model* sniper)
{
	DefCannon* obj = nullptr;
	if (type == DefenceType::Hand) { obj = DefCannon::Create(ShotMode::Fixed, hand); }
	else if (type == DefenceType::Assault) { obj = DefCannon::Create(ShotMode::Fixed, assault); }
	else if (type == DefenceType::Sniper) { obj = DefCannon::Create(ShotMode::Fixed, sniper); }

	return obj;
}

void DefCannon::AutoShot(std::vector<Enemy*> ePos)
{
	if (mode != ShotMode::Auto) { return; }

	int No = 0;
	//エネミーがいないなら抜ける
	if (ePos.size() == 0) { return; }

	for (int i = 0; i < bullet.size(); i++)
	{
		//エネミーがいないなら抜ける
		if (ePos.size() == 0) { break; }

		float _distance = 1000;
		for (int j = 0; j < ePos.size(); j++)
		{
			float dis = distance(position, ePos[j]->GetPosition());
			if (dis <= _distance) {
				_distance = dis;
				No = j;
			}
		}

		//生きていない
		if (bullet[i]->GetStatus().isAlive == false) { continue; }
		//目標が定まっている
		if (bullet[i]->GetStatus().vecSet != false) { continue; }
		bullet[i]->SetVec(objectVector(ePos[No]->GetPosition(), bullet[i]->GetStatus().basePos));
		bullet[i]->SetVec(XMVector3Normalize(bullet[i]->GetStatus().vec));
		bullet[i]->SetVecSet(true);
	}
}

void DefCannon::FixedShot()
{
	if (mode != ShotMode::Fixed) { return; }

	for (int i = 0; i < bullet.size(); i++)
	{
		//生きていない
		if (bullet[i]->GetStatus().isAlive != false) { continue; }
		////目標が定まっている
		//if (bullet[i]->GetStatus().vecSet != false) { continue; }
		bullet[i]->SetVec(shotVec);
		bullet[i]->SetVec(XMVector3Normalize(bullet[i]->GetStatus().vec));
		bullet[i]->SetVecSet(true);
		bullet[i]->Update();
	}
}

XMMATRIX DefCannon::LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward)
{
	Vector3 z = Vector3(forward.x, forward.y, forward.z);//進行方向ベクトル（前方向）
	Vector3 up = Vector3(upward.x, upward.y, upward.z);  //上方向
	XMMATRIX rot;//回転行列
	Quaternion q = quaternion(0, 0, 0, 1);//回転クォータニオン
	Vector3 _z = { 0.0f,0.0f,-1.0f };//Z方向単位ベクトル
	Vector3 cross;

	float a;//角度保存用
	float b;//角度保存用
	float c;//角度保存用
	float d;//角度保存用

	cross = z.cross(_z);

	q.x = cross.x;
	q.y = cross.y;
	q.z = cross.z;

	q.w = sqrt(
		(z.length() * z.length())
		* (_z.length() * _z.length())) + z.dot(_z);

	//単位クォータニオン化

	q = normalize(q);
	q = conjugate(q);
	a = q.x;
	b = q.y;
	c = q.z;
	d = q.w;

	//任意軸回転
	XMVECTOR rq = { q.x,q.y,q.z,q.w };
	rot = XMMatrixRotationQuaternion(rq);

	this->useRotMat = true;
	return rot;
}
