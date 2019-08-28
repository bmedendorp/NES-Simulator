#include "CPU_6502.h"
#include <iostream>
#include <string>

using namespace std;

CPU_6502::CPU_6502(Memory* bus)
{
	this->bus = bus;
	pc = 0x0000;
}

bool CPU_6502::Clock()
{
	static uint8_t currentCycle = 0;

	if (currentCycle == 0)
	{
		if (bus)
		{
			cout << "PC : " << pc << endl;
			// Fetch next instruction
			OpCode opCode = opCodes[bus->Read(pc++)];
			// Determine address using refereced addressing mode
			(this->*opCode.mode)();
			// Perform the requested opcode instruction
			(this->*opCode.instruction)();
			currentCycle = opCode.cycles - 1;
			cout << opCodeString << " : " << addressString << endl;
			cout << "PC : " << pc << endl;
		}
	}
	else
	{
		currentCycle--;
	}

	if (currentCycle == 0)
		return true;
	else
		return false;
}

void CPU_6502::Step()
{
	while (!Clock());	// Keep clocking until instruction is finished
}

// ****************
// Addressing Modes
// ****************
bool CPU_6502::ZPX()		// Zero Page Indexed (X)
{
	if (bus)
	{
		address = (bus->Read(pc++) + regX) && 0x00FF;
		addressString = to_string(address) + " - Zero Page Indexed (X)";
	}
	return false;
}
bool CPU_6502::ZPY()		// Zero Page Indexed (Y)
{
	if (bus)
	{
		address = (bus->Read(pc++) + regY) && 0x00FF;
		addressString = to_string(address) + " - Zero Page Indexed (Y)";
	}
	return false;
}
bool CPU_6502::ABX()		// Absolute Indexed (X)
{
	if (bus)
	{
		address = (uint16_t)(bus->Read(pc++) + regX) | (uint16_t)bus->Read(pc++) << 8;
		addressString = to_string(address) + " - Absolute Indexed (X)";
		if ((address & 0x00FF) < regX)
			return true;
	}
	return false;
}
bool CPU_6502::ABY()		// Absolute Indexed (Y)
{
	if (bus)
	{
		address = (uint16_t)(bus->Read(pc++) + regY) | (uint16_t)bus->Read(pc++) << 8;
		addressString = to_string(address) + " - Absolute Indexed (Y)";
		if ((address & 0x00FF) < regY)
			return true;
	}
	return false;
}
bool CPU_6502::IZX()		// Indexed Indirect
{
	if (bus)
	{
		uint8_t pointer = bus->Read(pc++);
		address = (bus->Read(pointer++ + regX) & 0x00FF) | (uint16_t)(bus->Read(pointer + regX) & 0x00FFF) << 8;
		addressString = to_string(address) + " - Indexed Indirect";
	}
	return false;
}
bool CPU_6502::IZY()		// Indirect Indexed
{
	if (bus)
	{
		uint8_t pointer = bus->Read(pc++);
		address = (bus->Read(pointer++) | (uint16_t)(bus->Read(pointer) & 0x00FF) << 8) + regY;
		addressString = to_string(address) + " - Indirect Indexed";
		if ((address & 0x00FF) < regY)
			return true;
	}
	return false;
}
bool CPU_6502::IMP()		// Implicit
{
	addressString = "Implicit";
	return false;
}
bool CPU_6502::IMM()		// Immediate
{
	address = pc++;
	addressString = to_string(address) + " - Immediate";
	return false;
}
bool CPU_6502::ZP()			// Zero Page
{
	if (bus)
	{
		address = bus->Read(pc++);
		addressString = to_string(address) + " - Zero Page";
	}
	return false;
}
bool CPU_6502::ABS()		// Absolute
{
	if (bus)
	{
		address = bus->Read(pc++) | (uint16_t)(bus->Read(pc++) << 8);
		addressString = to_string(address) + " - Absolute";
	}
	return false;
}
bool CPU_6502::REL()		// Relative
{
	if (bus)
	{
		uint8_t offset = bus->Read(pc++);
		if (offset & 0x80)
		{
			// Offset is negative, subtract 2's compliment from pc
			offset ^= 0xFF;
			offset++;
			address = pc - offset;
			addressString = to_string(address) + " - Relative";
			if ((address & 0x00FF) > offset)
				return true;
		} 
		else
		{
			address = pc + offset;
			addressString = to_string(address) + " - Relative";
			if ((address & 0x00FF) < offset)
				return true;
		}
	}
	return false;
}
bool CPU_6502::IND()		// Indirect
{
	if (bus)
	{
		uint16_t pointer = bus->Read(pc++) | (uint16_t)(bus->Read(pc++) << 8);
		address = bus->Read(pointer++) | (uint16_t)(bus->Read(pointer) << 8);
		addressString = to_string(address) + " - Indirect";
	}
	return false;
}


// ************
// Instructions
// ************

// Logical and Arithmetic Commands
bool CPU_6502::ORA()
{
	opCodeString = "ORA";
	regA |= bus->Read(address);
	SetZ(regA == 0);
	SetN(regA & 0x80);
	return true;
}
bool CPU_6502::AND()
{
	opCodeString = "AND";
	regA &= bus->Read(address);
	SetZ(regA == 0);
	SetN(regA & 0x80);
	return true;
}
bool CPU_6502::EOR()
{
	opCodeString = "EOR";
	regA ^= bus->Read(address);
	SetZ(regA == 0);
	SetN(regA & 0x80);
	return true;
}
bool CPU_6502::ADC()
{
	opCodeString = "ADC";
	AddCarry(bus->Read(address));
	return true;
}
bool CPU_6502::SBC()
{
	opCodeString = "SBC";
	AddCarry(bus->Read(address) ^ 0xFF);		// SBC is identical to ADC (data EOR 0xFF) - One's complement
	return true;
}
bool CPU_6502::CMP()
{
	opCodeString = "CMP";
	Compare(regA, bus->Read(address));
	return true;
}
bool CPU_6502::CPX()
{
	opCodeString = "CPX";
	Compare(regX, bus->Read(address));
	return false;
}
bool CPU_6502::CPY()
{
	opCodeString = "CPY";
	Compare(regY, bus->Read(address));
	return false;
}
bool CPU_6502::DEC()
{
	opCodeString = "DEC";
	bus->Write(address, Decrement(bus->Read(address)));
	return false;
}
bool CPU_6502::DEX()
{
	opCodeString = "DEX";
	regX = Decrement(regX);
	return false;
}
bool CPU_6502::DEY()
{
	opCodeString = "DEY";
	regY = Decrement(regY);
	return false;
}
bool CPU_6502::INC()
{
	opCodeString = "INC";
	bus->Write(address, Decrement(bus->Read(address)));
	return false;
}
bool CPU_6502::INX()
{
	opCodeString = "INX";
	regX = Increment(regX);
	return false;
}
bool CPU_6502::INY()
{
	opCodeString = "INY";
	regY = Increment(regY);
	return false;
}
bool CPU_6502::ASL()
{
	opCodeString = "ASL";
	uint8_t data = bus->Read(address);
	SetC(data & 0x80);
	data = data << 1;
	bus->Write(address, data);
	SetN(data & 0x80);
	SetZ(data == 0);
	return false;
}
bool CPU_6502::ROL()
{
	opCodeString = "ROL";
	uint8_t data = bus->Read(address);
	uint8_t c = status.C;
	SetC(data & 0x80);
	data = (data << 1) | c;
	bus->Write(address, data);
	SetN(data & 0x80);
	SetZ(data == 0);
	return false;
}
bool CPU_6502::LSR()
{
	opCodeString = "LSR";
	uint8_t data = bus->Read(address);
	SetC(data & 0x01);
	data = data >> 1;
	bus->Write(address, data);
	SetN(data & 0x80);
	SetZ(data == 0);
	return false;
}
bool CPU_6502::ROR()
{
	opCodeString = "ROR";
	uint8_t data = bus->Read(address);
	uint8_t c = status.C << 7;
	SetC(data & 0x01);
	data = (data >> 1) | c;
	bus->Write(address, data);
	SetN(data & 0x80);
	SetZ(data == 0);
	return false;
}

// Move Commands
bool CPU_6502::LDA()
{
	opCodeString = "LDA";
	return false;
}
bool CPU_6502::STA()
{
	opCodeString = "STA";
	return false;
}
bool CPU_6502::LDX()
{
	opCodeString = "LDX";
	return false;
}
bool CPU_6502::STX()
{
	opCodeString = "STX";
	return false;
}
bool CPU_6502::LDY()
{
	opCodeString = "LDY";
	return false;
}
bool CPU_6502::STY()
{
	opCodeString = "STY";
	return false;
}
bool CPU_6502::TAX()
{
	opCodeString = "TAX";
	return false;
}
bool CPU_6502::TXA()
{
	opCodeString = "TXA";
	return false;
}
bool CPU_6502::TAY()
{
	opCodeString = "TAY";
	return false;
}
bool CPU_6502::TYA()
{
	opCodeString = "TYA";
	return false;
}
bool CPU_6502::TSX()
{
	opCodeString = "TSX";
	return false;
}
bool CPU_6502::TXS()
{
	opCodeString = "TXS";
	return false;
}
bool CPU_6502::PLA()
{
	opCodeString = "PLA";
	return false;
}
bool CPU_6502::PHA()
{
	opCodeString = "PHA";
	return false;
}
bool CPU_6502::PLP()
{
	opCodeString = "PLP";
	return false;
}
bool CPU_6502::PHP()
{
	opCodeString = "PHP";
	return false;
}

// Jump/Flag Commands
bool CPU_6502::BPL()
{
	opCodeString = "BPL";
	return false;
}
bool CPU_6502::BMI()
{
	opCodeString = "BMI";
	return false;
}
bool CPU_6502::BVC()
{
	opCodeString = "BVC";
	return false;
}
bool CPU_6502::BVS()
{
	opCodeString = "BVS";
	return false;
}
bool CPU_6502::BCC()
{
	opCodeString = "BCC";
	return false;
}
bool CPU_6502::BCS()
{
	opCodeString = "BCS";
	return false;
}
bool CPU_6502::BNE()
{
	opCodeString = "BNE";
	return false;
}
bool CPU_6502::BEQ()
{
	opCodeString = "BEQ";
	return false;
}
bool CPU_6502::BRK()
{
	opCodeString = "BRK";
	return false;
}
bool CPU_6502::RTI()
{
	opCodeString = "RTI";
	return false;
}
bool CPU_6502::JSR()
{
	opCodeString = "JSR";
	return false;
}
bool CPU_6502::RTS()
{
	opCodeString = "RTS";
	return false;
}
bool CPU_6502::JMP()
{
	opCodeString = "JMP";
	return false;
}
bool CPU_6502::BIT()
{
	opCodeString = "BIT";
	return false;
}
bool CPU_6502::CLC()
{
	opCodeString = "CLC";
	return false;
}
bool CPU_6502::SEC()
{
	opCodeString = "SEC";
	return false;
}
bool CPU_6502::CLD()
{
	opCodeString = "CLD";
	return false;
}
bool CPU_6502::SED()
{
	opCodeString = "SED";
	return false;
}
bool CPU_6502::CLI()
{
	opCodeString = "CLI";
	return false;
}
bool CPU_6502::SEI()
{
	opCodeString = "SEI";
	return false;
}
bool CPU_6502::CLV()
{
	opCodeString = "CLV";
	return false;
}
bool CPU_6502::NOP()
{
	opCodeString = "NOP";
	return false;
}

// Illegal Opcodes
bool CPU_6502::SLO()
{
	opCodeString = "SLO";
	return false;
}
bool CPU_6502::RLA()
{
	opCodeString = "RLA";
	return false;
}
bool CPU_6502::SRE()
{
	opCodeString = "SRE";
	return false;
}
bool CPU_6502::RRA()
{
	opCodeString = "RRA";
	return false;
}
bool CPU_6502::SAX()
{
	opCodeString = "SAX";
	return false;
}
bool CPU_6502::LAX()
{
	opCodeString = "LAX";
	return false;
}
bool CPU_6502::DCP()
{
	opCodeString = "DCP";
	return false;
}
bool CPU_6502::ISC()
{
	opCodeString = "ISC";
	return false;
}
bool CPU_6502::ANC()
{
	opCodeString = "ANC";
	return false;
}
bool CPU_6502::ALR()
{
	opCodeString = "ALR";
	return false;
}
bool CPU_6502::ARR()
{
	opCodeString = "ARR";
	return false;
}
bool CPU_6502::XAA()
{
	opCodeString = "XAA";
	return false;
}
bool CPU_6502::AXS()
{
	opCodeString = "AXS";
	return false;
}
bool CPU_6502::AHX()
{
	opCodeString = "AHX";
	return false;
}
bool CPU_6502::SHY()
{
	opCodeString = "SHY";
	return false;
}
bool CPU_6502::SHX()
{
	opCodeString = "SHX";
	return false;
}
bool CPU_6502::TAS()
{
	opCodeString = "TAS";
	return false;
}
bool CPU_6502::LAS()
{
	opCodeString = "LAS";
	return false;
}
bool CPU_6502::KIL()
{
	opCodeString = "KIL";
	return false;
}

void CPU_6502::AddCarry(uint8_t data)	// Implemented in one place for both ADC and SBC
{
	bool checkV = (regA & 0x80) == (data & 0x80);	// Both numbers have same sign?
	regA += data + status.C;
	SetV(checkV && ((regA & 0x80) != (data & 0x80)));
	SetC(regA < data);
	SetZ(regA == 0);
	SetN(regA & 0x80);
}

void CPU_6502::Compare(uint8_t a, uint8_t b)	// Implemented in one place for CMP,CPX and CPY
{
	uint8_t res = a - b;
	SetN(res & 0x80);
	SetZ(res == 0);
	SetC(a < res);
}

uint8_t CPU_6502::Decrement(uint8_t a)
{
	a--;
	SetN(a & 0x80);
	SetZ(a == 0);
	return a;
}

uint8_t CPU_6502::Increment(uint8_t a)
{
	a++;
	SetN(a & 0x80);
	SetZ(a == 0);
	return a;
}



// ***************
// Status Register
// ***************
inline bool CPU_6502::SetN(bool n)
{
	return status.N = n;
}
inline bool CPU_6502::SetV(bool v)
{
	return status.V = v;
}
inline bool CPU_6502::SetD(bool d)
{
	return status.D = d;
}
inline bool CPU_6502::SetI(bool i)
{
	return status.I = i;
}
inline bool CPU_6502::SetZ(bool z)
{
	return status.Z = z;
}
inline bool CPU_6502::SetC(bool c)
{
	return status.C = c;
}
