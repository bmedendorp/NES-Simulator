#include "CPU_6502.h"
#include "Bus.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <chrono>

using namespace std;

CPU_6502::CPU_6502(Bus* bus)
{
	if (!bus)
		throw std::invalid_argument("Invalid NULL argument");
	this->bus = bus;

	Reset();
}

CPU_6502::~CPU_6502()
{
}

void CPU_6502::Reset()
{
	pc = bus->Read(0xFFFC) | (uint16_t)(bus->Read(0xFFFD) << 8);
	sp = 0xFF;
	regA = regX = regY = 0x00;
	currentInterrupt = InterruptType::INTERRUPT_NONE;
}

bool CPU_6502::Clock()
{
	static uint8_t currentCycle = 0;

	if (currentCycle == 0)
	{
		if (currentInterrupt == INTERRUPT_NONE)
		{
			// Fetch next instruction
			OpCode opCode = opCodes[bus->Read(pc++)];
			// Determine address using refereced addressing mode
			bImplied = false;
			bool extraCycle1 = (this->*opCode.mode)();
			// Perform the requested opcode instruction
			bool extraCycle2 = (this->*opCode.instruction)();
			currentCycle = opCode.cycles - 1;
			if (extraCycle1 & extraCycle2)
				currentCycle++;
		}
		else
		{
			OpCode opCode = { &CPU_6502::INT, &CPU_6502::IMP, 7, "INT", &CPU_6502::IMP_dis };	// Interrupt handling 'instruction'
			// Set address for proper interrupt handling routine
			switch (currentInterrupt)
			{
			case INTERRUPT_IRQ:
				address = 0xFFFE;
				break;
			case INTERRUPT_NMI:
				address = 0xFFFA;
				break;
			default:
				throw std::logic_error("Bad interrupt type");
			}
			// Handle the interrupt
			(this->*opCode.instruction)();
			currentCycle = opCode.cycles - 1;
			currentInterrupt = INTERRUPT_NONE;
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
	return false;
}

void CPU_6502::Step()
{
	while (!Clock());	// Keep clocking until instruction is finished
}

void CPU_6502::IRQ()
{
	if (!status.I)
		currentInterrupt = INTERRUPT_IRQ;
}

void CPU_6502::NMI()
{
	currentInterrupt = INTERRUPT_NMI;
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

const std::vector<CPU_6502::DisassembledInstruction>* CPU_6502::Disassemble(uint16_t startAddress, uint16_t size)
{
	disassembleInfo.clear();
	instructionMap.clear();
	CPU_6502::OpCode instruction;
	CPU_6502::DisassembledInstruction instructionInfo;
	int index = 0;

	uint16_t counter = startAddress;
	while (counter < startAddress + size)
	{
		instructionMap.emplace(counter, index);
		instructionInfo.address = counter;
		instruction = opCodes[bus->Read(counter++)];
		instructionInfo.instructionString = std::string(instruction.opCode) + " " + (this->*instruction.mode_dis)(counter);
		instructionInfo.instructionSize = counter - instructionInfo.address;
		disassembleInfo.push_back(instructionInfo);
	}

	return &disassembleInfo;
}

// ****************
// Addressing Modes
// ****************
bool CPU_6502::ZPX()		// Zero Page Indexed (X)
{
	uint8_t base = bus->Read(pc++);
	address = (base + regX) && 0x00FF;
	return false;
}
string CPU_6502::ZPX_dis(uint16_t& counter)
{
	uint8_t base = bus->Read(counter++);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << base;
	return string("$" + ss.str() + ",X");
}
bool CPU_6502::ZPY()		// Zero Page Indexed (Y)
{
	uint8_t base = bus->Read(pc++);
	address = (base + regY) && 0x00FF;
	//stringstream ss;
	//ss << uppercase << setfill('0') << setw(2) << hex << base;
	//operandString = "$" + ss.str() + ",Y";
	return false;
}
string CPU_6502::ZPY_dis(uint16_t& counter)
{
	uint8_t base = bus->Read(counter++);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << base;
	return string("$" + ss.str() + ",Y");
}
bool CPU_6502::ABX()		// Absolute Indexed (X)
{
	uint8_t lsb = bus->Read(pc++);
	uint16_t msb = (uint16_t)bus->Read(pc++);
	address = (lsb + regX) | (msb << 8);
	if ((address & 0x00FF) < regX)
		return true;
	return false;
}
string CPU_6502::ABX_dis(uint16_t& counter)
{
	uint8_t lsb = bus->Read(counter++);
	uint16_t msb = (uint16_t)bus->Read(counter++);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(4) << hex << (lsb | (msb << 8));
	return string("$" + ss.str() + ",X");
}
bool CPU_6502::ABY()		// Absolute Indexed (Y)
{
	uint8_t lsb = bus->Read(pc++);
	uint16_t msb = (uint16_t)bus->Read(pc++);
	address = (lsb + regY) | (msb << 8);
	if ((address & 0x00FF) < regY)
		return true;
	return false;
}
string CPU_6502::ABY_dis(uint16_t& counter)
{
	uint8_t lsb = bus->Read(counter++);
	uint16_t msb = (uint16_t)bus->Read(counter++);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(4) << hex << (lsb | (msb << 8));
	return string("$" + ss.str() + ",Y");
}
bool CPU_6502::IZX()		// Indexed Indirect
{
	uint8_t pointer = bus->Read(pc++);
	address = (bus->Read(pointer++ + regX) & 0x00FF) | (uint16_t)(bus->Read(pointer-- + regX) & 0x00FFF) << 8;
	return false;
}
string CPU_6502::IZX_dis(uint16_t& counter)
{
	uint8_t pointer = bus->Read(counter++);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << (int)pointer;
	return string("($" + ss.str() + ",X)");
}
bool CPU_6502::IZY()		// Indirect Indexed
{
	uint8_t pointer = bus->Read(pc++);
	address = (bus->Read(pointer++) | (uint16_t)(bus->Read(pointer--) & 0x00FF) << 8) + regY;
	if ((address & 0x00FF) < regY)
		return true;
	return false;
}
string CPU_6502::IZY_dis(uint16_t& counter)
{
	uint8_t pointer = bus->Read(counter++);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << (int)pointer;
	return string("($" + ss.str() + "),Y");
}
bool CPU_6502::IMP()		// Implicit
{
	bImplied = true;
	return false;
}
string CPU_6502::IMP_dis(uint16_t& counter)
{
	return string("");
}
bool CPU_6502::IMM()		// Immediate
{
	address = pc++;
	return false;
}
string CPU_6502::IMM_dis(uint16_t& counter)
{
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << (int)bus->Read(counter++);
	return string("#$" + ss.str());
}
bool CPU_6502::ZP()			// Zero Page
{
	address = bus->Read(pc++);
	return false;
}
string CPU_6502::ZP_dis(uint16_t& counter)
{
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << bus->Read(counter++);
	return string("$" + ss.str());
}
bool CPU_6502::ABS()		// Absolute
{
	address = bus->Read(pc++) | (uint16_t)(bus->Read(pc++) << 8);
	return false;
}
string CPU_6502::ABS_dis(uint16_t& counter)
{
	stringstream ss;
	ss << uppercase << setfill('0') << setw(4) << hex << (bus->Read(counter++) | (uint16_t)(bus->Read(counter++) << 8));
	return string("$" + ss.str());
}
bool CPU_6502::REL()		// Relative
{
	uint8_t offset = bus->Read(pc++);
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
	return false;
}
string CPU_6502::REL_dis(uint16_t& counter)
{
	uint8_t offset = bus->Read(counter++);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << (uint16_t)offset;
	return string("$" + ss.str());
}
bool CPU_6502::IND()		// Indirect
{
	uint16_t pointer = bus->Read(pc++) | (uint16_t)(bus->Read(pc++) << 8);
	address = bus->Read(pointer++) | (uint16_t)(bus->Read(pointer) << 8);
	return false;
}

string CPU_6502::IND_dis(uint16_t& counter)
{
	uint16_t pointer = bus->Read(counter++) | (uint16_t)(bus->Read(counter++) << 8);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(4) << hex << pointer;
	return string("($" + ss.str() + ")");
}


// ************
// Instructions
// ************

// Logical and Arithmetic Commands
bool CPU_6502::ORA()
{
	regA |= bus->Read(address);
	SetZ(regA == 0);
	SetN(regA & 0x80);
	return true;
}
bool CPU_6502::AND()
{
	regA &= bus->Read(address);
	SetZ(regA == 0);
	SetN(regA & 0x80);
	return true;
}
bool CPU_6502::EOR()
{
	regA ^= bus->Read(address);
	SetZ(regA == 0);
	SetN(regA & 0x80);
	return true;
}
bool CPU_6502::ADC()
{
	AddCarry(bus->Read(address));
	return true;
}
bool CPU_6502::SBC()
{
	AddCarry(bus->Read(address) ^ 0xFF + 1);		// Subtract is the same as add negative
	return true;
}
bool CPU_6502::CMP()
{
	Compare(regA, bus->Read(address));
	return true;
}
bool CPU_6502::CPX()
{
	Compare(regX, bus->Read(address));
	return false;
}
bool CPU_6502::CPY()
{
	Compare(regY, bus->Read(address));
	return false;
}
bool CPU_6502::DEC()
{
	bus->Write(address, Decrement(bus->Read(address)));
	return false;
}
bool CPU_6502::DEX()
{
	regX = Decrement(regX);
	return false;
}
bool CPU_6502::DEY()
{
	regY = Decrement(regY);
	return false;
}
bool CPU_6502::INC()
{
	bus->Write(address, Decrement(bus->Read(address)));
	return false;
}
bool CPU_6502::INX()
{
	regX = Increment(regX);
	return false;
}
bool CPU_6502::INY()
{
	regY = Increment(regY);
	return false;
}
bool CPU_6502::ASL()
{
	if (!bImplied)
	{
		uint8_t data = bus->Read(address);
		SetC(data & 0x80);
		data = data << 1;
		bus->Write(address, data);
		SetN(data & 0x80);
		SetZ(data == 0);
	}
	else
	{
		SetC(regA & 0x80);
		regA = regA << 1;
		SetN(regA & 0x80);
		SetZ(regA == 0);
	}
	return false;
}
bool CPU_6502::ROL()
{
	if (!bImplied)
	{
		uint8_t data = bus->Read(address);
		uint8_t c = status.C;
		SetC(data & 0x80);
		data = (data << 1) | c;
		bus->Write(address, data);
		SetN(data & 0x80);
		SetZ(data == 0);
	}
	else
	{
		uint8_t c = status.C;
		SetC(regA & 0x80);
		regA = (regA << 1) | c;
		SetN(regA & 0x80);
		SetZ(regA == 0);
	}
	return false;
}
bool CPU_6502::LSR()
{
	if (!bImplied)
	{
		uint8_t data = bus->Read(address);
		SetC(data & 0x01);
		data = data >> 1;
		bus->Write(address, data);
		SetN(data & 0x80);
		SetZ(data == 0);
	}
	else
	{
		SetC(regA & 0x01);
		regA = regA >> 1;
		SetN(regA & 0x80);
		SetZ(regA == 0);
	}
	return false;
}
bool CPU_6502::ROR()
{
	if (!bImplied)
	{
		uint8_t data = bus->Read(address);
		uint8_t c = status.C << 7;
		SetC(data & 0x01);
		data = (data >> 1) | c;
		bus->Write(address, data);
		SetN(data & 0x80);
		SetZ(data == 0);
	}
	else
	{
		uint8_t c = status.C << 7;
		SetC(regA & 0x01);
		regA = (regA >> 1) | c;
		SetN(regA & 0x80);
		SetZ(regA == 0);
	}
	return false;
}

// Move Commands
bool CPU_6502::LDA()
{
	regA = bus->Read(address);
	SetN(regA & 0x80);
	SetZ(regA == 0);
	return true;
}
bool CPU_6502::STA()
{
		bus->Write(address, regA);
	return false;
}
bool CPU_6502::LDX()
{
	regX = bus->Read(address);
	SetN(regX & 0x80);
	SetZ(regX == 0);
	return true;
}
bool CPU_6502::STX()
{
	bus->Write(address, regX);
	return false;
}
bool CPU_6502::LDY()
{
	regY = bus->Read(address);
	SetN(regY & 0x80);
	SetZ(regY == 0);
	return true;
}
bool CPU_6502::STY()
{
	bus->Write(address, regY);
	return false;
}
bool CPU_6502::TAX()
{
	regX = regA;
	SetN(regX & 0x80);
	SetZ(regX == 0);
	return false;
}
bool CPU_6502::TXA()
{
	regA = regX;
	SetN(regA & 0x80);
	SetZ(regA == 0);
	return false;
}
bool CPU_6502::TAY()
{
	regY = regA;
	SetN(regY & 0x80);
	SetZ(regY == 0);
	return false;
}
bool CPU_6502::TYA()
{
	regA = regY;
	SetN(regA & 0x80);
	SetZ(regA == 0);
	return false;
}
bool CPU_6502::TSX()
{
	regX = sp;
	SetN(regX & 0x80);
	SetZ(regX == 0);
	return false;
}
bool CPU_6502::TXS()
{
	sp = regX;
	return false;
}
bool CPU_6502::PLA()
{
	regA = bus->Read(++sp | 0x0100);
	SetN(regA & 0x80);
	SetZ(regA == 0);
	return false;
}
bool CPU_6502::PHA()
{
	bus->Write(sp-- | 0x0100, regA);
	return false;
}
bool CPU_6502::PLP()
{
	status.p = bus->Read(++sp | 0x0100);
	return false;
}
bool CPU_6502::PHP()
{
	status.B = 0x11;
	bus->Write(sp-- | 0x0100, status.p);
	return false;
}

// Jump/Flag Commands
bool CPU_6502::BPL()
{
	if (!status.N)
		pc = address;
	return true;
}
bool CPU_6502::BMI()
{
	if (status.N)
		pc = address;
	return true;
}
bool CPU_6502::BVC()
{
	if (!status.V)
		pc = address;
	return true;
}
bool CPU_6502::BVS()
{
	if (status.V)
		pc = address;
	return true;
}
bool CPU_6502::BCC()
{
	if (!status.C)
		pc = address;
	return true;
}
bool CPU_6502::BCS()
{
	if (status.C)
		pc = address;
	return true;
}
bool CPU_6502::BNE()
{
	if (!status.Z)
		pc = address;
	return true;
}
bool CPU_6502::BEQ()
{
	if (status.Z)
		pc = address;
	return true;
}
bool CPU_6502::BRK()
{
	status.B = 0x11;
	bus->Write(sp-- | 0x0100, (pc & 0xFF00) >> 8);
	bus->Write(sp-- | 0x0100, pc & 0x00FF);
	bus->Write(sp-- | 0x0100, status.p);
	pc = bus->Read(0xFFFE) | (bus->Read(0xFFFF) << 8);
	status.I = 1;
	return false;
}
bool CPU_6502::INT()
{
	status.B = 0x10;
	bus->Write(sp-- | 0x0100, (pc & 0xFF00) >> 8);
	bus->Write(sp-- | 0x0100, pc & 0x00FF);
	bus->Write(sp-- | 0x0100, status.p);
	pc = bus->Read(address) | (bus->Read(address + 1) << 8);
	status.I = 1;
	return false;
}
bool CPU_6502::RTI()
{
	status.p = bus->Read(++sp | 0x0100);
	pc = bus->Read(++sp | 0x0100) | (bus->Read(++sp | 0x0100) << 8);
	return false;
}
bool CPU_6502::JSR()
{
	bus->Write(sp-- | 0x0100, (pc & 0xFF00) >> 8);
	bus->Write(sp-- | 0x0100, pc & 0x00FF);
	pc = address;
	return false;
}
bool CPU_6502::RTS()
{
	pc = bus->Read(++sp | 0x0100) | (bus->Read(++sp | 0x0100) << 8);
	return false;
}
bool CPU_6502::JMP()
{
	pc = address;
	return false;
}
bool CPU_6502::BIT()
{
	SetN(bus->Read(address) & 0x80);
	SetV(bus->Read(address) & 0x40);
	SetZ(bus->Read(address) & regA);
	return false;
}
bool CPU_6502::CLC()
{
	SetC(0);
	return false;
}
bool CPU_6502::SEC()
{
	SetC(1);
	return false;
}
bool CPU_6502::CLD()
{
	SetD(0);
	return false;
}
bool CPU_6502::SED()
{
	SetD(1);
	return false;
}
bool CPU_6502::CLI()
{
	SetI(0);
	return false;
}
bool CPU_6502::SEI()
{
	SetI(1);
	return false;
}
bool CPU_6502::CLV()
{
	SetV(0);
	return false;
}
bool CPU_6502::NOP()
{
	return false;
}

// Illegal Opcodes
bool CPU_6502::SLO()
{
	return false;
}
bool CPU_6502::RLA()
{
	return false;
}
bool CPU_6502::SRE()
{
	return false;
}
bool CPU_6502::RRA()
{
	return false;
}
bool CPU_6502::SAX()
{
	return false;
}
bool CPU_6502::LAX()
{
	return false;
}
bool CPU_6502::DCP()
{
	return false;
}
bool CPU_6502::ISC()
{
	return false;
}
bool CPU_6502::ANC()
{
	return false;
}
bool CPU_6502::ALR()
{
	return false;
}
bool CPU_6502::ARR()
{
	return false;
}
bool CPU_6502::XAA()
{
	return false;
}
bool CPU_6502::AXS()
{
	return false;
}
bool CPU_6502::AHX()
{
	//opCodeString = "AHX";
	return false;
}
bool CPU_6502::SHY()
{
	return false;
}
bool CPU_6502::SHX()
{
	return false;
}
bool CPU_6502::TAS()
{
	return false;
}
bool CPU_6502::LAS()
{
	return false;
}
bool CPU_6502::KIL()
{
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
