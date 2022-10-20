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
	Sprite::Update();
}

void Fade::Draw()
{
	Sprite::Draw();
}

void Fade::FadeIn()
{
	SetAlpha(_alpha);
	_alpha += fadeNum;
	if (_alpha >= 1.0f) { fadeIn = false; }
	Update();
}

void Fade::FadeOut()
{
	SetAlpha(_alpha);
	_alpha -= fadeNum * 2;
	if (_alpha <= 0.0f) { fadeOut = false; }
	Update();
}

void Fade::halfFade()
{
	SetAlpha(_alpha);
	_alpha += fadeNum * 1.5f;
	if (_alpha >= 0.7f) { _alpha = 0.7f; }
}
