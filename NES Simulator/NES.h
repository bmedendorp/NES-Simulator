#pragma once
#include "olcPixelGameEngine.h"
#include "Bus.h"
#include "Memory.h"
#include "CPU_6502.h"
#include "PPU.h"
#include "NESLoader.h"

class NES : public olc::PixelGameEngine
{
public:
	NES();

private:
	CPU_6502* cpu;	
	Bus* bus;
	PPU* ppu;
	Memory* memory;
	NESLoader* loader;

public:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;

private:
	void DumpMemory(int32_t x, int32_t y, uint16_t memAddress, uint8_t width, uint8_t height);
	void DisplayRegisters(int32_t x, int32_t y);
	void DisplayCode(int32_t x, int32_t y, const CPU_6502::DisassembleInfo* data, uint8_t lines, uint16_t pc);
};
