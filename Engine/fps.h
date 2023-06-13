#pragma once
//#include <winnt.h>

//#include<mmsystem.h>
#include <windows.h>
#pragma comment(lib,"winmm.lib")

class Fps {
public:
	const float FREAM_TIME = 1.0f / 60.0f;
	float time = 0;
	double fps = 0;

	LARGE_INTEGER Freq = {};
	LARGE_INTEGER StartTime = {};
	LARGE_INTEGER NowTime = {};

	int iCount = 0;
	DWORD SleepTime = 0;
public:
	Fps(const Fps& heap) = delete;
	Fps& operator=(const Fps& heap) = delete;

	void Init();
	void PreRun();
	void PostRun();

private:
	Fps() = default;
	~Fps() {};
};