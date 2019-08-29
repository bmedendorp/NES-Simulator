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
	ram[1] = 0x00;
	ram[2] = 0x0F;
	ram[3] = 0x49;
	ram[4] = 0xFF;
	ram[5] = 0x49;
	ram[6] = 0xFF;
	ram[7] = 0xE8;
	ram[8] = 0xFD;
	ram[9] = 0x00;
	ram[10] = 0x0F;
	ram[11] = 0x65;
	ram[12] = 0x80;
	ram[13] = 0x6A;
	ram[14] = 0x6A;
	ram[15] = 0xC8;
	ram[16] = 0xC8;
	ram[17] = 0x51;
	ram[18] = 0xE2;
	ram[19] = 0xA9;
	ram[20] = 0xB0;
	ram[21] = 0x8D;
	ram[22] = 0x03;
	ram[23] = 0x0F;
	ram[0xE2] = 0x00;
	ram[0xE3] = 0x0F;
	ram[0x80] = 0x20;
	ram[0x0F00] = 0xAA;
	ram[0x0F01] = 0x80;
	ram[0x0F02] = 0x55;
}
