#include "Fade.h"

Fade::Fade(
	UINT texNumber,
	XMFLOAT2 position,
	XMFLOAT2 size,
	XMFLOAT4 color,
	XMFLOAT2 anchorpoint,
	bool isFlipX, bool isFlipY) :
	Sprite(
		texNumber,
		position,
		size,
		color,
		anchorpoint,
		isFlipX,
		isFlipY)
{
}

Fade* Fade::Create(UINT texNumber, XMFLOAT2 position, XMFLOAT4 color, XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	// 仮サイズ
	XMFLOAT2 size = { 100.0f, 100.0f };


	if (spritecommon._texBuff[texNumber])
	{
		// テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = spritecommon._texBuff[texNumber]->GetDesc();
		// スプライトのサイズをテクスチャのサイズに設定
		size = { (float)resDesc.Width, (float)resDesc.Height };
	}

	// Spriteのインスタンスを生成
	Fade* sprite = new Fade(texNumber, position, size, color, anchorpoint, isFlipX, isFlipY);
	if (sprite == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!sprite->Initalize()) {
		delete sprite;
		assert(0);
		return nullptr;
	}

	return sprite;
}

void Fade::Update()
{
	if (shake) { Shake(); }
	Sprite::Update();
}

void Fade::Draw()
{
	//if (this->alpha <= 0.0f) { return; }
	Sprite::Draw();
}

void Fade::FadeIn()
{
	if (!fadeIn) { return; }
	SetAlpha(_alpha);
	_alpha += fadeNum;
	if (_alpha >= 1.0f) {
		fadeIn = false;
		fadeOut = true;
	}
	Update();
}

void Fade::FadeOut()
{
	if (!fadeOut) { return; }
	SetAlpha(_alpha);
	_alpha -= fadeNum;
	if (_alpha <= 0.0f) {
		fadeOut = false;
	}
	Update();
}

void Fade::SlideOut()
{
	if (this->slideOut == true) {
		this->position.x = easing.easeIn(mainX, endX, easing.timeRate);
		easing.timeRate += easing.addTime;

		//イーズが終わったらタイムレートを０に
		if (easing.timeRate >= easing.maxTime) {
			easing.timeRate = easing.maxTime;
			this->slideOut = false;
		}
	}
}

void Fade::Shake()
{
	//リセット
	if (shakeTime <= 1.0f) {
		shakeTime = 20.0f;
		position = BasePos;
		PosDecision = false;
		shake = false;
		return;
	}

	//元座標決定
	if (!PosDecision) {
		BasePos = position;
		PosDecision = true;
	}

	//シェイク乱数決定
	shakeRand.x = rand() % int(shakeTime) - (int(shakeTime) / 3.0f);
	shakeRand.y = rand() % int(shakeTime) - (int(shakeTime) / 3.0f);

	//シェイク加算
	position.x = BasePos.x + shakeRand.x;
	position.y = BasePos.y + shakeRand.y;

	//シェイクタイム減算
	shakeTime -= 1.0f;
}

void Fade::SlideIn()
{
	if (this->slideIn == true) {
		this->position.x = easing.easeOut(startX, mainX, easing.timeRate);
		easing.timeRate += easing.addTime;

		//イーズが終わったらタイムレートを０に
		if (easing.timeRate >= easing.maxTime) {
			easing.timeRate = 0;
			this->slideIn = false;
			this->slideOut = true;
		}
	}
}

void Fade::halfFade()
{
	SetAlpha(_alpha);
	_alpha += fadeNum * 1.5f;
	if (_alpha >= 0.7f) { _alpha = 0.7f; }
}
