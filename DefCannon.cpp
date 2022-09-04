#include "DefCannon.h"

DefCannon::DefCannon()
{
}
DefCannon* DefCannon::Create(Model* model)
{
	// 3Dオブジェクトのインスタンスを生成
	DefCannon* object3d = new DefCannon();
	if (object3d == nullptr) {
		return nullptr;
	}

	object3d->position.y = 0;

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}
void DefCannon::BulletInit()
{
	for (int i = 0; i < 10; i++)
	{
		Bullet* _bullet;
		_bullet = _bullet->Create();
		_bullet->SetModel(model);
		_bullet->scale = { 1,1,1 };
		_bullet->status.isAlive = false;
		_bullet->status.speed = 1.0f;
		bullet.push_back(_bullet);
	}
}
void DefCannon::Update() {
	Object3Ds::Update();

	count += 1.0f / 60.0f;

	for (int i = 0; i < bullet.size(); i++) {
		if (bullet[i]->status.isAlive == false) {
			bullet[i]->status.basePos = position;
		}

		if (count >= bullet[i]->attackCount) {
			//一番初め(使われていなければ優先的に使う)
			if (i == 0 && bullet[i]->status.isAlive == false) {
				bullet[i]->status.isAlive = true;
				bullet[i]->Update();
				count = 0;//カウントをゼロに
				continue;
			}
			if (i != 0) {
				//ひとつ前の弾が撃たれていないとき
				if (bullet[i - 1]->status.isAlive == false) {
					bullet[i]->Update();
					continue;
				}
				//弾以前の弾が全て生きていて自分が生きていない
				else if(bullet[i]->status.isAlive == false) {
					bullet[i]->status.isAlive = true;
					count = 0;//カウントをゼロに
				}
			}
		}

		bullet[i]->Update();
	}
}
void DefCannon::Draw()
{
	Object3Ds::Draw();

	for (int i = 0; i < bullet.size(); i++) {
		bullet[i]->Draw();
	}
}

float	 DefCannon::distance(XMFLOAT3 pos1, XMFLOAT3 pos2)
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
XMVECTOR DefCannon::normalize(XMVECTOR vec)
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

void DefCannon::moveUpdate(std::vector<Enemy*> ePos)
{
	int No = 0;
	for (int i = 0; i < bullet.size(); i++)
	{
		//エネミーがいないなら抜ける
		if (ePos.size() == 0) { break; }

		float _distance = 1000;
		for (int j = 0; j < ePos.size(); j++)
		{
			float dis = distance(position, ePos[j]->position);
			if (dis <= _distance) {
				_distance = dis;
				No = j;
			}
		}

		//生きていない
		if (bullet[i]->status.isAlive == false) { continue; }
		//目標が定まっている
		if (bullet[i]->status.vecSet != false) { continue; }
		bullet[i]->status.vec = objectVector(ePos[No]->position, position);
		bullet[i]->status.vec = XMVector3Normalize(bullet[i]->status.vec);
		bullet[i]->status.vecSet = true;
	}
}
