#pragma once
#include <imgui/imgui.h>
#include <d3d12.h>

#include "dx12Wrapper.h"

class DebugImgui {
public:
	DebugImgui();
	~DebugImgui();

	static float shadowCameraSite[2];
	static float shadowlightPos[3];
	static float shadowlightTarget[3];

	static void UpdateImgui(Wrapper* dx12);
};
