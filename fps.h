#pragma once

//#include<mmsystem.h>
//#pragma comment(lib,"winmm.lib")

class Fps {
public:
	const float FREAM_TIME = 1.0f / 60.0f;
	float time = 0;
	float fps = 0;

public:
	Fps(const Fps& heap) = delete;
	Fps& operator=(const Fps& heap) = delete;

	void Run();

private:
	Fps() = default;
	~Fps() {};
};