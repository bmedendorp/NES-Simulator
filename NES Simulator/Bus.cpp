#include "Bus.h"
#include "BusDevice.h"
#include <cstdint>

Bus::Bus()
{
	for (int i = 0; i < 16; i++)
		devices[i] = NULL;
}

uint8_t Bus::Read(uint16_t address) const
{
	const BusDevice* const device = GetRegisteredDevice(address);
	if (device)
		return device->Read(address);
	return 0;	// Default value if address doesn't exist
}

void Bus::Write(uint16_t address, uint8_t data)
{
	BusDevice* const device = GetRegisteredDevice(address);
	if (device)
		device->Write(address, data);
}

bool Bus::RegisterDevice(BusDevice* device, uint16_t startAddress, uint8_t addressBlocks)
{
	if (addressBlocks < 1 || addressBlocks > 16)
		return false;

	int index = (startAddress & 0xF000) >> 12;

	// Check that the requested address space is valid and free
	for (int i = 0; i < addressBlocks; i++)
	{
		if (index + i >= 16)	// Requested address is out of range
			return false;
		if (devices[index + i] != NULL)	// Device already exists at this location
			return false;
	}

	// Assign requested address space to the device
	for (int i = 0; i < addressBlocks; i++)
	{
		devices[index + i] = device;
	}
}

BusDevice* Bus::GetRegisteredDevice(uint16_t address) const
{
	return devices[(address & 0xF000) >> 12];
}
