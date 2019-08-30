#pragma once
#include "BusDevice.h"
#include <cstdint>

class Memory : public BusDevice
{
public:
	Memory();
	~Memory();

private:
	const static uint16_t SIZE_16K = 0x4000;
	uint8_t* rom[2];

	uint8_t* GetBytePtr(uint16_t address) const;

public:
	uint8_t Read(uint16_t address) const override;
	void Write(uint16_t address, uint8_t data) override;
	void Initialize(uint8_t* mem);
	uint8_t* GetROMBuffer(bool highBank = false);
};

