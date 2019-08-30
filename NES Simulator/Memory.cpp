#include "Memory.h"

Memory::Memory() 
{
	rom[0] = new uint8_t[SIZE_16K];	// Lower 16k of ROM memory
	rom[1] = rom[0];				// Default mirror of lower 16k
	if (rom[0])
	{
		Initialize(rom[0]);
	}
}

Memory::~Memory()
{
	if (rom[1] != rom[0])
		delete[] rom[1];
	delete[] rom[0];
}

uint8_t Memory::Read(uint16_t address) const
{
	return *GetBytePtr(address);
}

void Memory::Write(uint16_t address, uint8_t data)
{
	*GetBytePtr(address) = data;
}

uint8_t* Memory::GetBytePtr(uint16_t address) const
{
	int index = address & 0x4000 ? 1 : 0;
	return &rom[index][address & 0x3FFF];
}

uint8_t* Memory::GetROMBuffer(bool highBank)
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

void Memory::Initialize(uint8_t * mem)
{
	for (int i = 0; i < 1024 * 4; i++)
	{

	}
}
