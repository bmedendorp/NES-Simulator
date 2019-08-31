#include "Memory.h"

Memory::Memory() 
{
	ram[3] = ram[2] = ram[1] = ram[0] = new uint8_t[SIZE_2K];	// 2K of internam ram mirrored to other banks
	if (ram[0])
	{
		Initialize(ram[0], SIZE_2K);
	}

	rom[1] = rom[0] = new uint8_t[SIZE_16K];	// Lower 16k of ROM memory (default is mirrored)
	if (rom[0])
	{
		Initialize(rom[0], SIZE_16K);
	}
}

Memory::~Memory()
{
	if (rom[1] != rom[0])
		delete[] rom[1];
	delete[] rom[0];
}

uint8_t Memory::Read(const uint16_t address) const
{
	return *GetBytePtr(address);
}

void Memory::Write(const uint16_t address, const uint8_t data)
{
	*GetBytePtr(address) = data;
}

uint8_t* Memory::GetBytePtr(const uint16_t address) const
{
	int index;

	// RAM or ROM address?
	if (address & 0xE000)	// ROM
	{
		index = address & 0x4000 ? 1 : 0;
		return &rom[index][address & 0x3FFF];
	}
	else					// RAM
	{
		index = (address & 0xF800) >> 11;
		return &ram[index][address & 0x07FF];
	}
}

uint8_t* Memory::GetROMBuffer(const bool highBank)
{
	if (highBank)
	{
		if (rom[1] == rom[0] || rom[1] == NULL)
		{
			// Allocate the upper 16k of memory
			rom[1] = new uint8_t[SIZE_16K];
		}
		return rom[1];
	}
	return rom[0];
}

void Memory::Initialize(uint8_t* mem, const uint16_t size)
{
	for (int i = 0; i < size; i++)
	{
		mem[i] = 0;
	}
}
