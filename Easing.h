#pragma once
#include "..\includes.h"

class Easing {

public:
	float start = -1280.0f;
	float end = -10.0f;
	float end2 = 10.0f;

	float maxTime = 2.0f;
	float timeRate = 0.0f;
	float addTime = 0.1f;

	float timeRate2 = 0.0f;
	float addTime2 = 0.1f;

	bool fadeFlag = false;

	float easeIn(const float startX, const float endX, const float time)
	{
		float y = time * time;
		return startX * (1.0f - y) + endX * y;
	}

	float easeOut(const float startX, const float endX, const float time)
	{
		float y = time * (2 - time);
		return startX * (1.0f - y) + endX * y;
	}

	void FadeReset() {
		addTime = 0.0f;
		addTime2 = 0.0f;
		timeRate = 0.0f;
		timeRate2 = 0.0f;
	}
};