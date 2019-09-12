#pragma once
#include "Memory.h"
#include "PPU.h"
#include <string>
#include <cstdint>

using namespace std;

class NESLoader
{
public:
	NESLoader(Memory* memory, PPU* ppu);

	bool LoadFile(string fileName);

private:
	Memory* memory;
	PPU* ppu;
};

