#pragma once
#include "Memory.h"
#include <string>
#include <cstdint>

using namespace std;

class NESLoader
{
public:
	NESLoader(Memory* bus);

	bool LoadFile(string fileName);

private:
	Memory* bus;
};

