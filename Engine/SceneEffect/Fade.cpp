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
	// ���T�C�Y
	XMFLOAT2 size = { 100.0f, 100.0f };

	if (spritecommon._texBuff[texNumber])
	{
		// �e�N�X�`�����擾
		D3D12_RESOURCE_DESC resDesc = spritecommon._texBuff[texNumber]->GetDesc();
		// �X�v���C�g�̃T�C�Y���e�N�X�`���̃T�C�Y�ɐݒ�
		size = { (float)resDesc.Width, (float)resDesc.Height };
	}

	// Sprite�̃C���X�^���X�𐶐�
	Fade* sprite = new Fade(texNumber, position, size, color, anchorpoint, isFlipX, isFlipY);
	if (sprite == nullptr) {
		return nullptr;
	}

	// ������
	if (!sprite->Initalize()) {
		delete sprite;
		assert(0);
		return nullptr;
	}

	return sprite;
}

void Fade::Reset()
{
	_alpha = 0.0f;
	SetAlpha(_alpha);
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
	_alpha += fadeNum;
	SetAlpha(_alpha);
	if (_alpha >= 1.0f) {
		SetAlpha(1.0f);
		fadeIn = false;
		fadeOut = true;
	}
	Update();
}

void Fade::FadeOut()
{
	if (!fadeOut) { return; }
	_alpha -= fadeNum;
	SetAlpha(_alpha);
	if (_alpha <= 0.0f) {
		SetAlpha(0.0f);
		fadeOut = false;
	}
	Update();
}

void Fade::SlideOut()
{
	if (this->slideOut == true) {
		this->position.x = easing.easeIn(mainX, endX, easing.timeRate);
		easing.timeRate += easing.addTime;

		//�C�[�Y���I�������^�C�����[�g���O��
		if (easing.timeRate >= easing.maxTime) {
			easing.timeRate = easing.maxTime;
			this->slideOut = false;
		}
	}
}

void Fade::UpIn()
{	
	if (GetUpIn()) { 
		return; 
	}

	static float teu = 0.0f;
	SetSize(XMFLOAT2(teu, 44.0f));
	Update();
	teu += 20.0f;
	if (teu >= 440.0f) {
		teu = 440.0f;
		SetDownOut(true);
		SetUpIn(false);
		SetAnchorPoint({ 1.0f,0.0f });
	}
}

void Fade::DownOut()
{
	if (GetDownOut()) {
		return;
	}

	static float teu = 440.0f;
	SetSize(XMFLOAT2(teu, 44.0f));
	Update();
	teu -= 20.0f;
	if (teu <= 0.0f) {
		teu = 0.0f;
		SetDownOut(false);
		SetAnchorPoint({ 0.0f,0.0f });
	}
}

void Fade::Shake()
{
	//���Z�b�g
	if (shakeTime <= 1.0f) {
		shakeTime = 20.0f;
		position = BasePos;
		PosDecision = false;
		shake = false;
		return;
	}

	//�����W����
	if (!PosDecision) {
		BasePos = position;
		PosDecision = true;
	}

	//�V�F�C�N��������
	shakeRand.x = rand() % int(shakeTime) - (int(shakeTime) / 3.0f);
	shakeRand.y = rand() % int(shakeTime) - (int(shakeTime) / 3.0f);

	//�V�F�C�N���Z
	position.x = BasePos.x + shakeRand.x;
	position.y = BasePos.y + shakeRand.y;

	//�V�F�C�N�^�C�����Z
	shakeTime -= 1.0f;
}

void Fade::SlideIn()
{
	if (this->slideIn == true) {
		this->position.x = easing.easeOut(startX, mainX, easing.timeRate);
		easing.timeRate += easing.addTime;

		//�C�[�Y���I�������^�C�����[�g���O��
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
