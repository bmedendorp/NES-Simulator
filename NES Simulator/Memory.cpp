#include "Memory.h"

Memory::Memory() 
{
	ram = new uint8_t[MEMORY_SIZE_BYTES];
	if (ram)
	{
		Initialize();
	}
}

Memory::~Memory()
{
	delete[] ram;
}

uint8_t Memory::Read(uint16_t address) const
{
	return ram[address];
}

void Memory::Write(uint16_t address, uint8_t data)
{
	ram[address] = data;
}

uint16_t Memory::GetBuffer(uint16_t address, uint8_t** bufPtr)
{
	if (ram)
	{
		*bufPtr = ram + address;
		return MEMORY_SIZE_BYTES - address + 1;
	}
	else
	{
		*bufPtr = NULL;
		return 0;
	}
}

void Memory::Initialize()
{
	// Program
	ram[0x0001] = 0x00;
	ram[0x0002] = 0x00;
}
