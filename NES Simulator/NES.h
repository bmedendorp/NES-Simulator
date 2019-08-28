#pragma once
#include "olcPixelGameEngine.h"
#include "Memory.h"
#include "CPU_6502.h"

class NES : public olc::PixelGameEngine
{
public:
	NES();

private:
	Memory *ram;
	CPU_6502 *cpu;

public:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
};
