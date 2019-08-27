#define OLC_PGE_APPLICATION
#include "NES.h"

NES::NES()
{
	sAppName = "NES Simulator";
	// Construct our 'physical' screen
	Construct(256, 240, 4, 4);
}

bool NES::OnUserCreate()
{
	// Called once at the start, so create things here
	return true;
}

bool NES::OnUserUpdate(float fElapsedTime)
{
	// called once per frame, draws random coloured pixels
	for (int x = 0; x < ScreenWidth(); x++)
		for (int y = 0; y < ScreenHeight(); y++)
			Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand() % 255));
	return true;
}
