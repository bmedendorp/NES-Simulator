#pragma once
#include "olcPixelGameEngine.h"
#include "Memory.h"

class NES : public olc::PixelGameEngine
{
public:
	NES();

private:
	Memory ram();

public:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
};
