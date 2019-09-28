#pragma once
#include "BusDevice.h"
#include "olcPixelGameEngine.h"
#include <cstdint>

#define PPUCTRL		0
#define PPUMASK		1
#define PPUSTATUS	2
#define OAMADDR		3
#define OAMDATA		4
#define PPUSCROLL	5
#define PPUADDR		6
#define PPUDATA		7

class PPU : public BusDevice
{
public:
	PPU();
	~PPU();

	enum NametableMapType { NAMETABLE_MAP_VERTICAL, NAMETABLE_MAP_HORIZONTAL, NAMETABLE_MAP_ONESCREEN, NAMETABLE_MAP_FOURSCREEN };

	void Reset();
	uint8_t Read(uint16_t address) const override;
	void Write(uint16_t address, uint8_t data) override;
	uint8_t* GetChrROMBuffer();
	const olc::Sprite* GetScreen() const;
	bool Clock();
	const olc::Sprite* GetPatternTable(uint8_t palette, bool left = true) const;
	olc::Pixel GetPaletteColor(int palette, int index) const;

private:
	olc::Sprite screen[2];
	int backBuffer;

	int16_t scanline;
	int16_t cycle;

	uint8_t registers[8];
	uint8_t colorData[28];
	uint8_t OAM[256];
	uint8_t* chrROM;
	uint8_t* videoRAM;
	uint8_t* videoRAM2;		// Pointer for additional video RAM if needed (4-Screen mapping)

	uint8_t* patternTable0;
	uint8_t* patternTable1;
	uint8_t* nametable0;
	uint8_t* nametable1;
	uint8_t* nametable2;
	uint8_t* nametable3;
	uint8_t* paletteRAM[32] = {&colorData[0x00], &colorData[0x01], &colorData[0x02], &colorData[0x03],
								 &colorData[0x19], &colorData[0x04], &colorData[0x05], &colorData[0x06],
								 &colorData[0x1A], &colorData[0x07], &colorData[0x08], &colorData[0x09],
								 &colorData[0x1B], &colorData[0x0A], &colorData[0x0B], &colorData[0x0C],
								 &colorData[0x00], &colorData[0x0D], &colorData[0x0E], &colorData[0x0F],
								 &colorData[0x19], &colorData[0x10], &colorData[0x11], &colorData[0x12],
								 &colorData[0x1A], &colorData[0x13], &colorData[0x14], &colorData[0x15],
								 &colorData[0x1B], &colorData[0x16], &colorData[0x17], &colorData[0x18]};
	olc::Pixel colors[64] = { {84, 84, 84}, {0, 30, 116}, {8, 16, 144}, {48, 0, 136}, {68, 0, 100}, {92, 0, 48}, {84, 4, 0}, {60, 24, 0}, {32, 42, 0}, {8, 58, 0}, {0, 64, 0}, {0, 60, 0}, {0, 50, 60}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
							{152, 150, 152}, {8, 76, 196}, {48, 50, 236}, {92, 30, 228}, {136, 20, 176}, {160, 20, 100}, {152, 34, 32}, {120, 60, 0}, {84, 90, 0}, {40, 114, 0}, {8, 124, 0}, {0, 118, 40}, {0, 102, 120}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
							{236, 238, 236}, {76, 154, 236}, {120, 124, 236}, {176, 98, 236}, {228, 84, 236}, {236, 88, 180},  {236, 106, 100}, {212, 136, 32}, {160, 170, 0}, {116, 196, 0}, {76, 208, 32}, {56, 204, 108}, {56, 180, 204}, {60, 60, 60}, {0, 0, 0}, {0, 0, 0},
							{236, 238, 236}, {168, 204, 236}, {188, 188, 236}, {212, 178, 236}, {236, 174, 236}, {236, 174, 212}, {236, 180, 176}, {228, 196, 144}, {204, 210, 120}, {180, 222, 120}, {168, 226, 144}, {152, 226, 180}, {160, 214, 228}, {160, 162, 160}, {0, 0, 0}, {0, 0, 0} };

	NametableMapType nametableType = NAMETABLE_MAP_VERTICAL;

	void MapNametables(NametableMapType type);
	uint8_t PPURead(uint16_t address) const;
	void PPUWrite(uint16_t address, uint8_t data);
	uint8_t* GetAddressPtr(uint16_t address) const;
};

