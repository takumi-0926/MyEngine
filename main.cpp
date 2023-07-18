
#include "HundredBeast.h"
#include "light\Light.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Framework* game = new HundredBeast();

	game->Run();

	delete game;

	return 0;
}