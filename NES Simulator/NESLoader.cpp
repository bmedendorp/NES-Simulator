#include "NESLoader.h"
#include "Memory.h"
#include <string>
#include <fstream>
#include <cstdint>
#include <stdexcept>

using namespace std;

NESLoader::NESLoader(Memory* memory, PPU* ppu)
{
	if (!memory | !ppu)
		throw std::invalid_argument("Invalid NULL argument");

	this->memory = memory;
	this->ppu = ppu;
}

bool NESLoader::LoadFile(string fileName)
{
	char header[16];
	bool result = true;

	// Open File for reading
	ifstream inFile;
	inFile.open(fileName, ios::in | ios::binary);
	if (!inFile.is_open())
		return false;

	// Read the header
	inFile.seekg(0, ios::beg);
	inFile.read(header, 16);

	// Determine the size of the program ROM (16 or 32K)
	bool b32k = header[4] == 2 ? true : false;

	// Determine if we're using CHR ROM
	bool bChrROM = header[5] ? true : false;

	// Determine if 'trainer' is present in file
	if (header[6] & 0x08)
		inFile.seekg(512, ios::cur);

	// Read the program data into memory
	char* memoryBuffer = NULL;
	memoryBuffer = (char*)memory->GetROMBuffer();	// Get pointer to memory location 0x8000 (start of program memory)
	if (memoryBuffer)
	{
		// Read the first 16k of program ROM
		inFile.read(memoryBuffer, 0x4000);
	}
	else
		result = false;

	if (b32k)
	{
		memoryBuffer = (char*)memory->GetROMBuffer(true);	// Get the pointer to memory location 0xC000 (2nd 16k of program memory)
		if (memoryBuffer)
		{
			// Read the 2nd 16k of program ROM
			inFile.read(memoryBuffer, 0x4000);
		}
		else
			result = false;
	}

	// Read CHR ROM data into the PPU
	if (bChrROM)
	{
		memoryBuffer = (char*)ppu->GetChrROMBuffer();
		if (memoryBuffer)
		{
			inFile.read(memoryBuffer, 0x2000);
		}
		else
			result = false;
	}

	inFile.close();
	return result;
}

