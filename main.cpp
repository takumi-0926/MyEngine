
#include "HundredBeast.h"

#include "gameManager.h"
#include "application.h"
#include "dx12Wrapper.h"
#include "Singleton_Heap.h"
#include "Audio/audio.h"
#include "Input/input.h"
#include "delete.h"
#include "object/baseObject.h"

#include "object/object3D.h"
#include "object/object2d.h"
#include "BillboardObject.h"

#include "object/Model.h"

#include "Sprite/sprite.h"
#include "2d/PostEffect.h"

#include "PMD/PMDmodel.h"
#include "PMD/pmdObject3D.h"
#include "PMD/PMXLoader.h"

#include "FBX/FbxLoader.h"
#include "FBX/FbxModel.h"
#include "FBX/FbxObject3d.h"

#include "Camera\DebugCamera.h"

#include "light\Light.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	HundredBeast game;
	game.Initialize();

	//ÉQÅ[ÉÄÉãÅ[Év
	while (true) {
		game.Update();

		if (game.IsEndReqest()) {
			break;
		}

		game.Draw();
	}

#pragma region å„èàóù

	game.Finalize();

#pragma endregion

	return 0;
}