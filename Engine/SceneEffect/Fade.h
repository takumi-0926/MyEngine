#pragma once
#include "Sprite/sprite.h"
#include "Easing.h"
#include "application.h"

class Easing;
class Fade : public Sprite {
private:
	float AddTime = 1.0f / 60.0f;
	//�t�F�[�h�֌W
	float fadeTime = 60;
	float fadeNum = 1 / fadeTime;
	bool fadeIn = false;
	bool fadeOut = false;
	bool halffade = false;
	bool clear = false;
	bool failed = false;

	float _alpha = 0.0f;

	//�C�[�W���O�֌W
	Easing easing;
	float startX = 0;//�X�^�[�g�ʒu
	float mainX = Application::window_width / 2.0f;//���ԕ\���ʒu
	float endX = Application::window_width;//�I���ʒu

	bool slideIn = true;
	bool slideOut = false;
	float waitTime = 1.0f;

	bool upIn = true;
	bool downOut = false;

	//�V�F�C�N�֌W
	float shakeTime = 20.0f;//�V�F�C�N����
	XMFLOAT2 BasePos;//�V�F�C�N�����W
	XMFLOAT2 shakeRand;//�V�F�C�N���Z�l
	bool PosDecision = false; //���W����
	bool shake = false;
public:
	Fade(UINT texNumber,
		XMFLOAT2 position,
		XMFLOAT2 size,
		XMFLOAT4 color,
		XMFLOAT2 anchorpoint,
		bool isFlipX, bool isFlipY);

	static Fade* Create(
		UINT texNumber,
		XMFLOAT2 position,
		XMFLOAT4 color = { 1,1,1,1 },
		XMFLOAT2 anchorpoint = { 0.0f,0.0f },
		bool isFlipX = false, bool isFlipY = false);

	void Reset();

	void Update()override;
	void Draw()override;

	//�t�F�[�h�C�� / �A�E�g
	void FadeIn();
	void FadeOut();
	void halfFade();

	//�C�[�Y�C�� / �A�E�g
	void SlideIn();
	void SlideOut();

	void UpIn();
	void DownOut();

	//�V�F�C�N
	void Shake();
public:
	float GetAlpha() { return alpha; }

	bool GetFadeIn() { return fadeIn; }
	bool GetFadeOut() { return fadeOut; }
	bool GethalfFade() { return halffade; }
	bool GetClear() { return clear; }
	bool GetFailed() { return failed; }
	bool GetSlideOut() { return slideOut; }
	bool GetShake() { return slideOut; }
	bool GetUpIn() { return upIn; }
	bool GetDownOut() { return downOut; }
	void SetFadeIn(bool _flag) { this->fadeIn = _flag; }
	void SetFadeOut(bool _flag) { this->fadeOut = _flag; }
	void SethalfFade(bool _flag) { this->halffade = _flag; }
	void SetClear(bool _flag) { this->clear = _flag; }
	void SetFailed(bool _flag) { this->failed = _flag; }
	void SetShake(bool _flag) { this->shake = _flag; }
	void SetUpIn(bool _flag) { this->upIn = _flag; }
	void SetDownOut(bool _flag) { this->downOut = _flag; }
};