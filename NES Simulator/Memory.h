#pragma once
#include "BusDevice.h"
#include <cstdint>

#define MEMORY_SIZE_BYTES	0xFFFF

class Memory : public BusDevice
{
public:
	Memory();
	~Memory();

private:
	uint8_t* ram = NULL;

public:
	uint8_t Read(uint16_t address) const override;
	void Write(uint16_t address, uint8_t data) override;
	void Initialize();
	uint16_t GetBuffer(uint16_t address, uint8_t **bufPtr);
};

