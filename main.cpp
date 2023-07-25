
#include "HundredBeast.h"
#include <windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Framework* game = new HundredBeast();

	game->Run();

	delete game;

	return 0;
}