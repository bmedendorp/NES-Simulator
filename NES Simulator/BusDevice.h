#pragma once
#include <cstdint>

class BusDevice
{
public:
	virtual uint8_t Read(uint16_t address) const = 0;
	virtual void Write(uint16_t address, uint8_t data) = 0;
	
protected:
	const static uint16_t SIZE_1K = 0x0400;
	const static uint16_t SIZE_2K = 0x0800;
	const static uint16_t SIZE_4K = 0x1000;
	const static uint16_t SIZE_8K = 0x2000;
	const static uint16_t SIZE_16K = 0x4000;
};

