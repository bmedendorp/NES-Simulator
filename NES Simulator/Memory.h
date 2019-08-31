#pragma once
#include "BusDevice.h"
#include <cstdint>

class Memory : public BusDevice
{
public:
	Memory();
	~Memory();

private:
	uint8_t* ram[4];
	uint8_t* rom[2];

	uint8_t* GetBytePtr(uint16_t address) const;

public:
	uint8_t Read(const uint16_t address) const override;
	void Write(const uint16_t address, const uint8_t data) override;
	void Initialize(uint8_t* mem, const uint16_t size);
	uint8_t* GetROMBuffer(const bool highBank = false);
};

