#define OLC_PGE_APPLICATION
#include "NES.h"
#include "Memory.h"
#include "CPU_6502.h"
#include <iostream>

using namespace std;

NES::NES()
{
	sAppName = "NES Simulator";
	// Construct our 'physical' screen
	Construct(256, 240, 4, 4);
	ram = new Memory();
	if (ram)
	{
		cpu = new CPU_6502(ram);
	}
	if (cpu)
	{
		CPU_6502::DisassembleInfo disassembleInfo[8];
		uint8_t instructionCount = cpu->Disassemble(0x0000, 8, disassembleInfo);
		for (int i = 0; i < instructionCount; i++)
		{
			cout << disassembleInfo[i].instructionString << endl;
		}
	}
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
