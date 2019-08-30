#pragma once
#include "BusDevice.h"
#include <cstdint>

class Bus
{
public:
	Bus();
	~Bus();

	uint8_t Read(uint16_t address) const;
	void Write(uint16_t address, uint8_t data);
	bool RegisterDevice(BusDevice* device, uint16_t startAddress, uint8_t addressBlocks = 1);	// Each address block is 4k (16 blocks in total for 64k)

private:
	BusDevice* devices[16];

	BusDevice* GetRegisteredDevice(uint16_t address) const;
};

