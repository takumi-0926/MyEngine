#pragma once
#include "Sprite/sprite.h"
class Fade : public Sprite {
private:
	float fadeTime = 60;
	float fadeNum = 1 / fadeTime;
	bool fadeIn = false;
	bool fadeOut = false;
	bool halffade = false;
	bool clear = false;
	bool failed = false;
	float _alpha = 0.0f;

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

	void Update()override;
	void Draw()override;

	void FadeIn();
	void FadeOut();

	void halfFade();
public:
	float GetAlpha() { return alpha; }

	bool GetFadeIn() { return fadeIn; }
	bool GetFadeOut() { return fadeOut; }
	bool GethalfFade() { return halffade; }
	bool GetClear() { return clear; }
	bool GetFailed() { return failed; }
	void SetFadeIn(bool _flag) { this->fadeIn = _flag; }
	void SetFadeOut(bool _flag) { this->fadeOut = _flag; }
	void SethalfFade(bool _flag) { this->halffade = _flag; }
	void SetClear(bool _flag) { this->clear = _flag; }
	void SetFailed(bool _flag) { this->failed = _flag; }
};