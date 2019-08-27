#include "Memory.h"

Memory::Memory() 
{
	ram = new uint8_t[MEMORY_SIZE_BYTES];
}

Memory::~Memory()
{
	delete[] ram;
}

uint8_t Memory::Read(uint32_t address) const
{
	return ram[address];
}

void Memory::Write(uint32_t address, uint8_t data)
{
	ram[address] = data;
}