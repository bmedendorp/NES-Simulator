#pragma once
#include <cstdint>

#define MEMORY_SIZE_BYTES	0xFFFF

class Memory
{
public:
	Memory();
	~Memory();

private:
	uint8_t* ram = NULL;

public:
	uint8_t Read(uint16_t address) const;
	void Write(uint16_t address, uint8_t data);
};

