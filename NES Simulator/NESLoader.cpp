#include "NESLoader.h"
#include "Memory.h"
#include <string>
#include <fstream>
#include <cstdint>

using namespace std;

NESLoader::NESLoader(Memory* bus)
{
	this->bus = bus;
}

bool NESLoader::LoadFile(string fileName)
{
	char header[16];

	// Open File for reading
	ifstream inFile;
	inFile.open(fileName, ios::in | ios::binary);
	if (!inFile.is_open())
		return false;

	// Read the header
	inFile.seekg(0, ios::beg);
	inFile.read(header, 16);

	// Determine the size of the program data (header byte 4 * 16k)
	uint16_t prgDataSize = 1024 * 16 * header[4];

	// Read the program data into memory
	char* memoryBuffer = NULL;
	uint16_t size = bus->GetBuffer(0x8000, (uint8_t**)&memoryBuffer);	// Get pointer to memory location 0x8000 (start of program memory)
	if (size >= 1024 * 32)	// 32k
	{
		// Determine if 'trainer' is present in file
		if (header[6] & 0x08)
			inFile.seekg(512, ios::cur);

		streampos prgStart = inFile.tellg();

		// Read the program data
		inFile.read(memoryBuffer, prgDataSize);

		// If only 16k of data, mirror it into the next 16k of memory
		if (header[4] == 1)
		{
			inFile.seekg(prgStart);
			inFile.read(memoryBuffer + 1024 * 16, prgDataSize);
		}
		inFile.close();
		return true;
	}
	inFile.close();
	return false;
}

