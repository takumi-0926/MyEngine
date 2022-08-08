#include "testObj.h"

Obj::Obj()
{
	pos.clear(0);
	r = R_OBJ;
	dist = 0;
	vel.clear(0);
	acc.clear(0);
	m = 1.0f;
	stat = 0;
	idx_hist = 0;
	//	hit = 0;
	//	hit_cnt = 0;
	for (int i = 0; i < NUM_HIST; i++)
	{
		hp[i].clear(0);
	}
	link0 = NULL;
	link1 = NULL;
	grabbed = 0;

	// 初期位置は軸の真下から左方向に45度傾いた位置
	x = CLENGTH / 8.0;

	// 初速度は０
	speed = 0.0;
}

Obj* Obj::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	Obj* object3d = new Obj();
	if (object3d == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}

bool Obj::Initialize()
{
	Object3Ds::Initialize();
	return true;
}

void Obj::Update()
{
	Object3Ds::Update();


	//hp[idx_hist % NUM_HIST].x = pos.x;
	//hp[idx_hist % NUM_HIST].y = pos.y;
	//idx_hist++;

	//pos.x = position.x;
	//pos.y = position.y;

	////if (grabbed)
	////{
	////	pos = MousePos;
	////	if (!MouseInputLeft)
	////		grabbed = 0;
	////}
	////else
	////{
	////	double _dst = pos.distance(MousePos);
	////	if (MouseInputLeft)
	////	{
	////		if (_dst < r)
	////			grabbed = 1;
	////	}

	////}

	//// 0:存在してない　1:存在して活動中　2:存在してて活動してない
	//if (stat == 2)
	//	return;

	//{
	//	// acc:加速度
	//	acc.x = 0.0f;
	//	acc.y = ACC_G;	//重力加速度（値は適当で良い）

	//	if (link0)	//親OBJがある?
	//	{
	//		// 自分の1つ上のobjへのベクトル
	//		Vec2	_dist = link0->pos - pos;
	//		// ↑のベクトルの大きさ
	//		double	_len = _dist.length();

	//		// 親objとの距離が標準距離（LENGTH_LINK）より大きければ
	//		// 加速度を加算
	//		if (_len > LENGTH_LINK)
	//		{
	//			// ベクトル(_dist)＊標準距離からの増分
	//			_dist = _dist * (_len - LENGTH_LINK);

	//			_dist /= LENGTH_LINK;

	//			// K_HOOK:ばね定数（伸びにくさ）
	//			// 質量(m)が大きいほど_distを掛けた加速度(acc)の増分は減る
	//			Vec2 add_acc = _dist * K_HOOK / m;
	//			acc += add_acc;
	//		}
	//	}
	//	if (link1)	//子OBJがある?
	//	{
	//		// 自分の１つ下のobjへのベクトル
	//		Vec2	_dist = link1->pos - pos;
	//		// ↑のベクトルの大きさ
	//		double	_len = _dist.length();

	//		if (_len > LENGTH_LINK)
	//		{
	//			_dist = _dist * (_len - LENGTH_LINK) / LENGTH_LINK;
	//			acc += _dist * K_HOOK / m;
	//		}
	//	}

	//	vel += acc;			//速度+=加速度

	//	vel -= vel * K_V_RES;	//粘性抵抗
	//							// (物体が近傍の流体を引きずることによって受ける反作用)

	//	pos += vel;			//位置+=速度

	//	position.x = pos.x;
	//	position.y = pos.y;
	//}



		// 公式に従って速度を加算
		// MASSの値を小さくするとゆっくり動く
		speed += -MASS * G * sin(x / LENGTH);

		// 速度に従って円上の座標を変更
		x += speed;

		// 軸を原点とした場合のぶら下がっている物の座標を算出
		// このままだと－45～45度の振り子になるので
		// 時計回りに90度（PI/2.0）回転
		angle = x / LENGTH + PI / 2.0;

		// 求めた角度から軸を原点とした円周上の座標を取得
		nx = cos(angle) * LENGTH;
		ny = sin(angle) * LENGTH;

		position.x = jiku_x + nx;
		position.y = jiku_y + ny;
}

void Obj::Draw()
{
	Object3Ds::Draw();
}

void Obj::disp()
{
	double	x = pos.x + SX_SCREEN / 2;
	double	y = pos.y + SY_SCREEN / 2;

	//DrawCircle(x, y, r, GetColor(128, 128, 128), 1, 1);

	//linkの表示
	if (link0)
	{
		double	xl = link0->pos.x + SX_SCREEN / 2;
		double	yl = link0->pos.y + SY_SCREEN / 2;
		//DrawLine(x, y, xl, yl, GetColor(255, 255, 255));//線を描画
	}
}
