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

void Memory::Initialize()
{
	// ORA 0xABCD (43981)
	ram[0] = 0x0D;
	ram[1] = 0xCD;
	ram[2] = 0xAB;
	ram[3] = 0x00;
	ram[4] = 0x00;
	ram[5] = 0x00;
	ram[6] = 0x00;
	ram[7] = 0x00;
	ram[8] = 0x00;
	ram[9] = 0x02;
}
