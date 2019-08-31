#include "PPU.h"
#include <cstdint>
#include <stdexcept>

PPU::PPU()
{
	registers[PPUCTRL] = 0x00;
	registers[PPUMASK] = 0x00;
	registers[PPUSTATUS] = 0xA0;
	registers[OAMADDR] = 0x00;
	registers[PPUSCROLL] = 0x00;
	registers[PPUADDR] = 0x00;
	registers[PPUDATA] = 0x00;

	chrROM = new uint8_t[SIZE_16K];
	videoRAM = new uint8_t[SIZE_2K];
	videoRAM2 = NULL;

	patternTable0 = chrROM;
	patternTable1 = chrROM + SIZE_4K;
	MapNametables(nametableType);
}

PPU::~PPU()
{
	delete[] chrROM;
	delete[] videoRAM;
	if (videoRAM2)
		delete[] videoRAM2;
}

void PPU::Reset()
{
	registers[PPUCTRL] = 0x00;
	registers[PPUMASK] = 0x00;
	registers[PPUSTATUS] &= 0x80;
	//registers[OAMADDR] = registers[OAMADDR];	// Effectively unchanged
	registers[PPUSCROLL] = 0x00;
	//registers[PPUADDR] = registers[PPUADDR];	// Unchanged
	registers[PPUDATA] = 0x00;
}

uint8_t PPU::Read(uint16_t address) const
{
	return registers[address & 0x0007];		// The same 8 register bytes are mirrored across the entire 8k of address space
}

void PPU::Write(uint16_t address, uint8_t data)
{
	registers[address & 0x0007] = data;		// The same 8 register bytes are mirrored across the entire 8k of address space
}

void PPU::MapNametables(NametableMapType type)
{
	switch (type)
	{
	case NAMETABLE_MAP_VERTICAL:
		nametable0 = nametable2 = videoRAM;
		nametable1 = nametable3 = videoRAM + SIZE_1K;
		break;

	case NAMETABLE_MAP_HORIZONTAL:
		nametable0 = nametable1 = videoRAM;
		nametable2 = nametable3 = videoRAM + SIZE_1K;
		break;

	case NAMETABLE_MAP_ONESCREEN:
		nametable0 = nametable1 = nametable2 = nametable3 = videoRAM;
		break;

	case NAMETABLE_MAP_FOURSCREEN:
		// Allocate additional memory for nametables
		videoRAM2 = new uint8_t[SIZE_2K];
		nametable0 = videoRAM;
		nametable1 = videoRAM + SIZE_1K;
		nametable2 = videoRAM2;
		nametable3 = videoRAM2 + SIZE_1K;
		break;

	default:
		throw std::invalid_argument("Invalid NametableMapType");
	}
}

uint8_t PPU::PPURead(uint16_t address) const
{
	return *GetAddressPtr(address);
}

void PPU::PPUWrite(uint16_t address, uint8_t data)
{
	*GetAddressPtr(address) = data;
}

uint8_t* PPU::GetAddressPtr(uint16_t address) const
{
	uint16_t offset = address & 0x0FFF;
	switch ((address & 0x3000) >> 12)
	{
	case 0:
		return &patternTable0[offset];
		
	case 1:
		return &patternTable1[offset];

	case 3:
		offset = address & 0x1F;
		switch ((address & 0x0100) >> 8)
		{
		case 1:
			return paletteRAM[offset];

		//case 0:
			// Falls through to mirror nametable addresses
		}

	case 2:
		offset = address & 0x03FF;
		switch ((address & 0x0C00) >> 10)
		{
		case 0:
			return &nametable0[offset];

		case 1:
			return &nametable1[offset];

		case 2:
			return &nametable2[offset];

		case 3:
			return &nametable3[offset];
		}
		
	default:
		throw std::logic_error("Invalid address mapping");
	}
}
