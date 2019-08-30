#pragma once
#include <cstdint>

class BusDevice
{
public:
	virtual uint8_t Read(uint16_t address) const = 0;
	virtual void Write(uint16_t address, uint8_t data) = 0;
};

