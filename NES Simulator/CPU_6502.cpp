#include "CPU_6502.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

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
			// Fetch next instruction
			OpCode opCode = opCodes[bus->Read(pc++)];
			// Determine address using refereced addressing mode
			(this->*opCode.mode)();
			// Perform the requested opcode instruction
			(this->*opCode.instruction)(false);
			currentCycle = opCode.cycles - 1;
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

uint8_t CPU_6502::GetA() const
{
	return regA;
}

uint8_t CPU_6502::GetX() const
{
	return regX;
}

uint8_t CPU_6502::GetY() const
{
	return regY;
}

uint8_t CPU_6502::GetStatus() const
{
	return status.p;
}

uint16_t CPU_6502::GetProgramCounter() const
{
	return pc;
}

uint8_t CPU_6502::GetStackPointer() const
{
	return sp;
}

uint8_t CPU_6502::Disassemble(uint16_t programStart, uint8_t instructionCount, CPU_6502::DisassembleInfo* data, uint16_t maxBytes)
{
	// Save program counter
	uint16_t originalPC = pc;

	pc = programStart;
	int i;
	for (i = 0; i < instructionCount; i++)
	{
		data[i].address = pc;
		OpCode opCode = opCodes[bus->Read(pc++)];
		(this->*opCode.mode)();				// Get the arguments (address) string
		(this->*opCode.instruction)(true);	// Get the opcode
		if ((maxBytes && (pc - originalPC > maxBytes)) || pc < originalPC)
			break;
		data[i].instructionSize = pc - data[i].address;
		data[i].instructionString = opCodeString + " " + operandString;
	}
	pc = originalPC;	// Restore original program counter
	return i;
}


// ****************
// Addressing Modes
// ****************
bool CPU_6502::ZPX()		// Zero Page Indexed (X)
{
	if (bus)
	{
		uint8_t base = bus->Read(pc++);
		address = (base + regX) && 0x00FF;
		stringstream ss;
		ss << uppercase << setfill('0') << setw(2) << hex << base;
		operandString = "$" + ss.str() + ",X";
	}
	return false;
}
bool CPU_6502::ZPY()		// Zero Page Indexed (Y)
{
	if (bus)
	{
		uint8_t base = bus->Read(pc++);
		address = (base + regY) && 0x00FF;
		stringstream ss;
		ss << uppercase << setfill('0') << setw(2) << hex << base;
		operandString = "$" + ss.str() + ",Y";
	}
	return false;
}
bool CPU_6502::ABX()		// Absolute Indexed (X)
{
	if (bus)
	{
		uint8_t lsb = bus->Read(pc++);
		uint16_t msb = (uint16_t)bus->Read(pc++);
		address = (lsb + regX) | (msb << 8);
		stringstream ss;
		ss << uppercase << setfill('0') << setw(4) << hex << (lsb | (msb << 8));
		operandString = "$" + ss.str() + ",X";
		if ((address & 0x00FF) < regX)
			return true;
	}
	return false;
}
bool CPU_6502::ABY()		// Absolute Indexed (Y)
{
	if (bus)
	{
		uint8_t lsb = bus->Read(pc++);
		uint16_t msb = (uint16_t)bus->Read(pc++);
		address = (lsb + regY) | (msb << 8);
		stringstream ss;
		ss << uppercase << setfill('0') << setw(4) << hex << (lsb | (msb << 8));
		operandString = "$" + ss.str() + ",Y";
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
		address = (bus->Read(pointer++ + regX) & 0x00FF) | (uint16_t)(bus->Read(pointer-- + regX) & 0x00FFF) << 8;
		stringstream ss;
		ss << uppercase << setfill('0') << setw(2) << hex << pointer;
		operandString = "($" + ss.str() + ",X)";
	}
	return false;
}
bool CPU_6502::IZY()		// Indirect Indexed
{
	if (bus)
	{
		uint8_t pointer = bus->Read(pc++);
		address = (bus->Read(pointer++) | (uint16_t)(bus->Read(pointer--) & 0x00FF) << 8) + regY;
		stringstream ss;
		ss << uppercase << setfill('0') << setw(2) << hex << pointer;
		operandString = "($" + ss.str() + "),Y";
		if ((address & 0x00FF) < regY)
			return true;
	}
	return false;
}
bool CPU_6502::IMP()		// Implicit
{
	operandString = "";
	return false;
}
bool CPU_6502::IMM()		// Immediate
{
	if (bus)
	{
		address = pc++;
		stringstream ss;
		ss << uppercase << setfill('0') << setw(2) << hex << (int)bus->Read(address);
		operandString = "#" + ss.str();
	}
	return false;
}
bool CPU_6502::ZP()			// Zero Page
{
	if (bus)
	{
		address = bus->Read(pc++);
		stringstream ss;
		ss << uppercase << setfill('0') << setw(2) << hex << address;
		operandString = "$" + ss.str();
	}
	return false;
}
bool CPU_6502::ABS()		// Absolute
{
	if (bus)
	{
		address = bus->Read(pc++) | (uint16_t)(bus->Read(pc++) << 8);
		stringstream ss;
		ss << uppercase << setfill('0') << setw(4) << hex << address;
		operandString = "$" + ss.str();
	}
	return false;
}
bool CPU_6502::REL()		// Relative
{
	if (bus)
	{
		uint8_t offset = bus->Read(pc++);
		stringstream ss;
		ss << uppercase << setfill('0') << setw(2) << hex << address;
		operandString = "$" + ss.str();
		if (offset & 0x80)
		{
			// Offset is negative, subtract 2's compliment from pc
			offset ^= 0xFF;
			offset++;
			address = pc - offset;
			if ((address & 0x00FF) > offset)
				return true;
		} 
		else
		{
			address = pc + offset;
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
		stringstream ss;
		ss << uppercase << setfill('0') << setw(4) << hex << pointer;
		operandString = "($" + ss.str() + ")";
	}
	return false;
}


// ************
// Instructions
// ************

// Logical and Arithmetic Commands
bool CPU_6502::ORA(bool disassemble)
{
	opCodeString = "ORA";
	if (!disassemble)
	{
		regA |= bus->Read(address);
		SetZ(regA == 0);
		SetN(regA & 0x80);
	}
	return true;
}
bool CPU_6502::AND(bool disassemble)
{
	opCodeString = "AND";
	if (!disassemble)
	{
		regA &= bus->Read(address);
		SetZ(regA == 0);
		SetN(regA & 0x80);
	}
	return true;
}
bool CPU_6502::EOR(bool disassemble)
{
	opCodeString = "EOR";
	if (!disassemble)
	{
		regA ^= bus->Read(address);
		SetZ(regA == 0);
		SetN(regA & 0x80);
	}
	return true;
}
bool CPU_6502::ADC(bool disassemble)
{
	opCodeString = "ADC";
	if (!disassemble)
	{
		AddCarry(bus->Read(address));
	}
	return true;
}
bool CPU_6502::SBC(bool disassemble)
{
	opCodeString = "SBC";
	if (!disassemble)
	{
		AddCarry(bus->Read(address) ^ 0xFF);		// SBC is identical to ADC (data EOR 0xFF) - One's complement
	}
	return true;
}
bool CPU_6502::CMP(bool disassemble)
{
	opCodeString = "CMP";
	if (!disassemble)
	{
		Compare(regA, bus->Read(address));
	}
	return true;
}
bool CPU_6502::CPX(bool disassemble)
{
	opCodeString = "CPX";
	if (!disassemble)
	{
		Compare(regX, bus->Read(address));
	}
	return false;
}
bool CPU_6502::CPY(bool disassemble)
{
	opCodeString = "CPY";
	if (!disassemble)
	{
		Compare(regY, bus->Read(address));
	}
	return false;
}
bool CPU_6502::DEC(bool disassemble)
{
	opCodeString = "DEC";
	if (!disassemble)
	{
		bus->Write(address, Decrement(bus->Read(address)));
	}
	return false;
}
bool CPU_6502::DEX(bool disassemble)
{
	opCodeString = "DEX";
	if (!disassemble)
	{
		regX = Decrement(regX);
	}
	return false;
}
bool CPU_6502::DEY(bool disassemble)
{
	opCodeString = "DEY";
	if (!disassemble)
	{
		regY = Decrement(regY);
	}
	return false;
}
bool CPU_6502::INC(bool disassemble)
{
	opCodeString = "INC";
	if (!disassemble)
	{
		bus->Write(address, Decrement(bus->Read(address)));
	}
	return false;
}
bool CPU_6502::INX(bool disassemble)
{
	opCodeString = "INX";
	if (!disassemble)
	{
		regX = Increment(regX);
	}
	return false;
}
bool CPU_6502::INY(bool disassemble)
{
	opCodeString = "INY";
	if (!disassemble)
	{
		regY = Increment(regY);
	}
	return false;
}
bool CPU_6502::ASL(bool disassemble)
{
	opCodeString = "ASL";
	if (!disassemble)
	{
		uint8_t data = bus->Read(address);
		SetC(data & 0x80);
		data = data << 1;
		bus->Write(address, data);
		SetN(data & 0x80);
		SetZ(data == 0);
	}
	return false;
}
bool CPU_6502::ROL(bool disassemble)
{
	opCodeString = "ROL";
	if (!disassemble)
	{
		uint8_t data = bus->Read(address);
		uint8_t c = status.C;
		SetC(data & 0x80);
		data = (data << 1) | c;
		bus->Write(address, data);
		SetN(data & 0x80);
		SetZ(data == 0);
	}
	return false;
}
bool CPU_6502::LSR(bool disassemble)
{
	opCodeString = "LSR";
	if (!disassemble)
	{
		uint8_t data = bus->Read(address);
		SetC(data & 0x01);
		data = data >> 1;
		bus->Write(address, data);
		SetN(data & 0x80);
		SetZ(data == 0);
	}
	return false;
}
bool CPU_6502::ROR(bool disassemble)
{
	opCodeString = "ROR";
	if (!disassemble)
	{
		uint8_t data = bus->Read(address);
		uint8_t c = status.C << 7;
		SetC(data & 0x01);
		data = (data >> 1) | c;
		bus->Write(address, data);
		SetN(data & 0x80);
		SetZ(data == 0);
	}
	return false;
}

// Move Commands
bool CPU_6502::LDA(bool disassemble)
{
	opCodeString = "LDA";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::STA(bool disassemble)
{
	opCodeString = "STA";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::LDX(bool disassemble)
{
	opCodeString = "LDX";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::STX(bool disassemble)
{
	opCodeString = "STX";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::LDY(bool disassemble)
{
	opCodeString = "LDY";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::STY(bool disassemble)
{
	opCodeString = "STY";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::TAX(bool disassemble)
{
	opCodeString = "TAX";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::TXA(bool disassemble)
{
	opCodeString = "TXA";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::TAY(bool disassemble)
{
	opCodeString = "TAY";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::TYA(bool disassemble)
{
	opCodeString = "TYA";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::TSX(bool disassemble)
{
	opCodeString = "TSX";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::TXS(bool disassemble)
{
	opCodeString = "TXS";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::PLA(bool disassemble)
{
	opCodeString = "PLA";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::PHA(bool disassemble)
{
	opCodeString = "PHA";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::PLP(bool disassemble)
{
	opCodeString = "PLP";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::PHP(bool disassemble)
{
	opCodeString = "PHP";
	if (!disassemble)
	{

	}
	return false;
}

// Jump/Flag Commands
bool CPU_6502::BPL(bool disassemble)
{
	opCodeString = "BPL";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::BMI(bool disassemble)
{
	opCodeString = "BMI";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::BVC(bool disassemble)
{
	opCodeString = "BVC";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::BVS(bool disassemble)
{
	opCodeString = "BVS";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::BCC(bool disassemble)
{
	opCodeString = "BCC";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::BCS(bool disassemble)
{
	opCodeString = "BCS";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::BNE(bool disassemble)
{
	opCodeString = "BNE";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::BEQ(bool disassemble)
{
	opCodeString = "BEQ";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::BRK(bool disassemble)
{
	opCodeString = "BRK";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::RTI(bool disassemble)
{
	opCodeString = "RTI";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::JSR(bool disassemble)
{
	opCodeString = "JSR";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::RTS(bool disassemble)
{
	opCodeString = "RTS";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::JMP(bool disassemble)
{
	opCodeString = "JMP";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::BIT(bool disassemble)
{
	opCodeString = "BIT";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::CLC(bool disassemble)
{
	opCodeString = "CLC";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::SEC(bool disassemble)
{
	opCodeString = "SEC";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::CLD(bool disassemble)
{
	opCodeString = "CLD";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::SED(bool disassemble)
{
	opCodeString = "SED";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::CLI(bool disassemble)
{
	opCodeString = "CLI";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::SEI(bool disassemble)
{
	opCodeString = "SEI";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::CLV(bool disassemble)
{
	opCodeString = "CLV";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::NOP(bool disassemble)
{
	opCodeString = "NOP";
	if (!disassemble)
	{

	}
	return false;
}

// Illegal Opcodes
bool CPU_6502::SLO(bool disassemble)
{
	opCodeString = "SLO";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::RLA(bool disassemble)
{
	opCodeString = "RLA";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::SRE(bool disassemble)
{
	opCodeString = "SRE";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::RRA(bool disassemble)
{
	opCodeString = "RRA";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::SAX(bool disassemble)
{
	opCodeString = "SAX";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::LAX(bool disassemble)
{
	opCodeString = "LAX";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::DCP(bool disassemble)
{
	opCodeString = "DCP";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::ISC(bool disassemble)
{
	opCodeString = "ISC";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::ANC(bool disassemble)
{
	opCodeString = "ANC";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::ALR(bool disassemble)
{
	opCodeString = "ALR";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::ARR(bool disassemble)
{
	opCodeString = "ARR";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::XAA(bool disassemble)
{
	opCodeString = "XAA";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::AXS(bool disassemble)
{
	opCodeString = "AXS";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::AHX(bool disassemble)
{
	opCodeString = "AHX";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::SHY(bool disassemble)
{
	opCodeString = "SHY";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::SHX(bool disassemble)
{
	opCodeString = "SHX";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::TAS(bool disassemble)
{
	opCodeString = "TAS";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::LAS(bool disassemble)
{
	opCodeString = "LAS";
	if (!disassemble)
	{

	}
	return false;
}
bool CPU_6502::KIL(bool disassemble)
{
	opCodeString = "KIL";
	if (!disassemble)
	{

	}
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
