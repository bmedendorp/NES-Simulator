#include "PPU.h"
#include <cstdint>
#include <stdexcept>

PPU::PPU() : screen{ {256, 240}, {256, 240} }
{
	scanline = -1;
	cycle = 0;
	backBuffer = 0;

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

	for (int i = 0; i < 28; i++)
	{
		colorData[i] = 0;
	}
	*paletteRAM[0] = 0x0d;
	*paletteRAM[1] = 0x02;
	*paletteRAM[2] = 0x06;
	*paletteRAM[3] = 0x0a;
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
	scanline = -1;
	cycle = 0;

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

uint8_t* PPU::GetChrROMBuffer()
{
	return chrROM;
}

const olc::Sprite* PPU::GetScreen() const
{
	return &screen[(backBuffer + 1) % 2];
}

bool PPU::Clock()
{
	bool result = false;

	if (++cycle > 261)
	{
		if (++scanline > 340)
		{
			scanline = -1;
			backBuffer = (backBuffer + 1) % 2;
			result = true;
		}
		cycle = 0;
	}
	screen[backBuffer].SetPixel(cycle, scanline, rand() % 2 ? olc::BLACK : olc::WHITE);
	return result;
}

const olc::Sprite* PPU::GetPatternTable(uint8_t paletteIndex, bool left) const
{
	static olc::Sprite sprite(128, 128);
	olc::Pixel* display = sprite.GetData();
	uint8_t* palette = paletteRAM[paletteIndex * 4];
	uint8_t* patternTable = left ? patternTable0 : patternTable1;

	int tableIndex = 0;
	for (int i = 0; i < 128; i += 8)	// Row Iteration (8 rows per iteration)
	{
		for (int j = 0; j < 128; j += 8)	// Column Iteration (8 columns per iteration)
		{
			for (int k = 0; k < 8; k++, tableIndex++)
			{
				uint8_t lsb = patternTable[tableIndex];
				uint8_t msb = patternTable[tableIndex + 8];
				for (int l = 0; l < 8; l++)
				{
					uint8_t index = ((lsb & 0x80) >> 7) + ((msb & 0x80) >> 6);
					lsb <<= 1;
					msb <<= 1;
					display[(i + k) * 128 + (j + l)] = colors[*paletteRAM[paletteIndex * 4 + index]];
				}
			}
			tableIndex += 8;
		}
	}

	return &sprite;
}

olc::Pixel PPU::GetPaletteColor(int palette, int index) const
{
	uint8_t colorIndex = *paletteRAM[palette * 4 + index];
	return colors[colorIndex];
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
