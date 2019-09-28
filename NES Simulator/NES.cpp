#define OLC_PGE_APPLICATION
#include "NES.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

NES::NES()
{
	sAppName = "NES Simulator";
	// Construct our 'physical' screen
	Construct(800, 480, 2, 2);
	bus = new Bus();
	memory = new Memory();
	ppu = new PPU();
	if (bus)
	{
		if (ppu)
		{
			bus->RegisterDevice(ppu, 0x2000, 2);		// PPU Registers
		}
		if (memory)
		{
			bus->RegisterDevice(memory, 0x8000, 8);		// Program ROM
			bus->RegisterDevice(memory, 0x0000, 2);		// Internal RAM
		}
		loader = new NESLoader(memory, ppu);
		loader->LoadFile("F:\\nestest.nes");
		cpu = new CPU_6502(bus);
	}
}

bool NES::OnUserCreate()
{
	const vector<CPU_6502::DisassembledInstruction>* instructionList = cpu->Disassemble(0x8000, 0x0FFF);
	//for (int i = 0; i != instructionList->size(); i++)
	//{
	//	std::cout << instructionList->at(i).instructionString << std::endl;
	//}
	return true;
}

bool NES::OnUserUpdate(float fElapsedTime)
{
#ifdef DEBUG
	//olc::HWButton spaceStatus = GetKey(olc::Key::SPACE);
	//if (spaceStatus.bPressed && cpu && ppu)
	//{
	//	do
	//	{
	//		ppu->Clock();		// 3 ppu cycles for every cpu cycle
	//		ppu->Clock();
	//		ppu->Clock();
	//	} while (!cpu->Clock());
	//}
#else
	Clock();
#endif

	Clear(olc::BLUE);
	DrawSprite(0, 0, ppu->GetScreen(), 2);
	DisplayRegisters(520, 50);
	DrawSprite(523, 345, ppu->GetPatternTable(0, true), 1);
	DrawSprite(660, 345, ppu->GetPatternTable(0, false), 1);
	return true;
}

void NES::DumpMemory(int32_t x, int32_t y, uint16_t memAddress, uint8_t width, uint8_t height)
{
	// Memory Address Column
	for (int i = 0; i < height; i++)
	{
		stringstream ss;
		ss << uppercase << setfill('0') << setw(4) << hex << memAddress + i * width;
		DrawString(x, y + 10 * i, ss.str(), olc::DARK_GREY);
	}

	// Memory Contents Columns
	if (bus)
	{
		for (int i = 0; i < height; i++)
		{
			stringstream ss;
			for (int j = 0; j < width; j++)
			{
				ss << uppercase << setfill('0') << setw(2) << hex << (int)bus->Read(memAddress + i * width + j) << " ";
			}
			DrawString(x + 40, y + 10 * i, ss.str(), olc::WHITE);
		}
	}
}

void NES::DisplayRegisters(int32_t x, int32_t y)
{
	if (cpu)
	{
		// A Register
		stringstream ss;
		ss << uppercase << setfill('0') << setw(2) << hex << (int)cpu->GetA();
		DrawString(x, y, "A: 0x" + ss.str());

		// X Register
		ss.str("");
		ss << uppercase << setfill('0') << setw(2) << hex << (int)cpu->GetX();
		DrawString(x, y + 10, "X: 0x" + ss.str());

		// Y Register
		ss.str("");
		ss << uppercase << setfill('0') << setw(2) << hex << (int)cpu->GetY();
		DrawString(x, y + 20, "Y: 0x" + ss.str());

		// Stack Pointer
		ss.str("");
		ss << uppercase << setfill('0') << setw(4) << hex << (int)cpu->GetStackPointer();
		DrawString(x + 72, y + 10, "SP: 0x" + ss.str());

		// Program Counter
		ss.str("");
		ss << uppercase << setfill('0') << setw(4) << hex << (int)cpu->GetProgramCounter();
		DrawString(x + 72, y + 20, "PC: 0x" + ss.str());

		// Status Flags (NVDIZC)
		ss.str("");
		DrawString(x, y + 35, "N", olc::CYAN);			// Negative Flag
		if (cpu->GetStatus() & 0x80)
			DrawString(x, y + 45, "X", olc::DARK_RED);
		else
			DrawString(x, y + 45, "0", olc::GREY);

		DrawString(x + 12, y + 35, "V", olc::CYAN);		// Overflow Flag
		if (cpu->GetStatus() & 0x40)
			DrawString(x + 12, y + 45, "X", olc::DARK_RED);
		else
			DrawString(x + 12, y + 45, "0", olc::GREY);

		DrawString(x + 24, y + 35, "D", olc::CYAN);		// Decimal Flag
		if (cpu->GetStatus() & 0x08)
			DrawString(x + 24, y + 45, "X", olc::DARK_RED);
		else
			DrawString(x + 24, y + 45, "0", olc::GREY);

		DrawString(x + 36, y + 35, "I", olc::CYAN);		// Interrupt Disable
		if (cpu->GetStatus() & 0x04)
			DrawString(x + 36, y + 45, "X", olc::DARK_RED);
		else
			DrawString(x + 36, y + 45, "0", olc::GREY);

		DrawString(x + 48, y + 35, "Z", olc::CYAN);		// Zero Flag
		if (cpu->GetStatus() & 0x02)
			DrawString(x + 48, y + 45, "X", olc::DARK_RED);
		else
			DrawString(x + 48, y + 45, "0", olc::GREY);

		DrawString(x + 60, y + 35, "C", olc::CYAN);		// Carry Flag
		if (cpu->GetStatus() & 0x01)
			DrawString(x + 60, y + 45, "X", olc::DARK_RED);
		else
			DrawString(x + 60, y + 45, "0", olc::GREY);
	}
}

//void NES::DisplayCode(int32_t x, int32_t y, const CPU_6502::DisassembleInfo* data, uint8_t lines, uint16_t pc)
//{
//	for (int i = 0; i < lines; i++)
//	{
//		DrawString(x, y + 10 * i, data->instructions[i].instructionString, data->instructions[i].address == pc ? olc::RED : olc::GREY);
//	}
//}

void NES::Clock()
{
	bool vSync = false;

	do
	{
		vSync = ppu->Clock();
		vSync |= ppu->Clock();
		vSync |= ppu->Clock();
		cpu->Clock();
	} while (!vSync);
}
