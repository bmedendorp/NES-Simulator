#include "CPU_6502.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

CPU_6502::CPU_6502(Memory* bus, uint8_t disassembleLines)
{
	this->bus = bus;

	maxDisassemblySize = disassembleLines;
	disassembleInfo.instructions = new DisassembledInstruction[maxDisassemblySize];
	Reset();
}

CPU_6502::~CPU_6502()
{
	if (disassembleInfo.instructions)
		delete[] disassembleInfo.instructions;
}

void CPU_6502::Reset()
{
	if (!bus)
		throw ExceptionType::EXCEPTION_NULL_BUS_PTR;

	pc = bus->Read(0xFFFC) | (uint16_t)(bus->Read(0xFFFD) << 8);
	sp = 0xFF;
	regA = regX = regY = 0x00;
	currentInterrupt = InterruptType::INTERRUPT_NONE;

	disassemblyIndex = 0;
	if (disassembleInfo.instructions)
	{
		disassembleInfo.count = Disassemble(pc, maxDisassemblySize, &disassembleInfo);
	}
}

bool CPU_6502::Clock()
{
	static uint8_t currentCycle = 0;

	if (currentCycle == 0)
	{
		if (!bus)
			throw ExceptionType::EXCEPTION_NULL_BUS_PTR;

		if (currentInterrupt == INTERRUPT_NONE)
		{
			AdvanceDisassembler();
			// Fetch next instruction
			OpCode opCode = opCodes[bus->Read(pc++)];
			// Determine address using refereced addressing mode
			bool extraCycle1 = (this->*opCode.mode)();
			// Perform the requested opcode instruction
			bool extraCycle2 = (this->*opCode.instruction)(false);
			currentCycle = opCode.cycles - 1;
			if (extraCycle1 & extraCycle2)
				currentCycle++;
		}
		else
		{
			// AdvanceDisassembler();
			OpCode opCode = { &CPU_6502::INT, &CPU_6502::IMP, 7 };	// Interrupt handling 'instruction'
			operandString = "";
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
				throw ExceptionType::EXCEPTION_INVALID_INTERRUPT;
			}
			// Handle the interrupt
			(this->*opCode.instruction)(false);
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

uint8_t CPU_6502::Disassemble(uint16_t programStart, uint8_t instructionCount, CPU_6502::DisassembleInfo* data, uint16_t maxBytes)
{
	if (!bus)
		throw ExceptionType::EXCEPTION_NULL_BUS_PTR;

	// Save program counter
	uint16_t originalPC = pc;

	pc = programStart;
	int i;
	for (i = 0; i < instructionCount; i++)
	{
		data->instructions[i].address = pc;
		OpCode opCode = opCodes[bus->Read(pc++)];
		(this->*opCode.mode)();				// Get the arguments (address) string
		(this->*opCode.instruction)(true);	// Get the opcode
		if ((maxBytes && (pc - originalPC > maxBytes)) || pc < programStart)
			break;
		data->instructions[i].instructionSize = pc - data->instructions[i].address;
		data->instructions[i].instructionString = opCodeString + " " + operandString;
	}
	pc = originalPC;	// Restore original program counter
	return data->count = i;
}

const CPU_6502::DisassembleInfo* CPU_6502::GetDisassembleInfo() const
{
	return &disassembleInfo;
}

void CPU_6502::AdvanceDisassembler()
{
	disassemblyIndex++;
	while (disassemblyIndex > disassembleInfo.count / 2 && disassembleInfo.count == maxDisassemblySize && disassembleInfo.instructions)
	{
		Disassemble(disassembleInfo.instructions[1].address, maxDisassemblySize, &disassembleInfo);
		disassemblyIndex--;
	}
}

// ****************
// Addressing Modes
// ****************
bool CPU_6502::ZPX()		// Zero Page Indexed (X)
{
	uint8_t base = bus->Read(pc++);
	address = (base + regX) && 0x00FF;
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << base;
	operandString = "$" + ss.str() + ",X";
	return false;
}
bool CPU_6502::ZPY()		// Zero Page Indexed (Y)
{
	uint8_t base = bus->Read(pc++);
	address = (base + regY) && 0x00FF;
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << base;
	operandString = "$" + ss.str() + ",Y";
	return false;
}
bool CPU_6502::ABX()		// Absolute Indexed (X)
{
	uint8_t lsb = bus->Read(pc++);
	uint16_t msb = (uint16_t)bus->Read(pc++);
	address = (lsb + regX) | (msb << 8);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(4) << hex << (lsb | (msb << 8));
	operandString = "$" + ss.str() + ",X";
	if ((address & 0x00FF) < regX)
		return true;
	return false;
}
bool CPU_6502::ABY()		// Absolute Indexed (Y)
{
	uint8_t lsb = bus->Read(pc++);
	uint16_t msb = (uint16_t)bus->Read(pc++);
	address = (lsb + regY) | (msb << 8);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(4) << hex << (lsb | (msb << 8));
	operandString = "$" + ss.str() + ",Y";
	if ((address & 0x00FF) < regY)
		return true;
	return false;
}
bool CPU_6502::IZX()		// Indexed Indirect
{
	uint8_t pointer = bus->Read(pc++);
	address = (bus->Read(pointer++ + regX) & 0x00FF) | (uint16_t)(bus->Read(pointer-- + regX) & 0x00FFF) << 8;
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << (int)pointer;
	operandString = "($" + ss.str() + ",X)";
	return false;
}
bool CPU_6502::IZY()		// Indirect Indexed
{
	uint8_t pointer = bus->Read(pc++);
	address = (bus->Read(pointer++) | (uint16_t)(bus->Read(pointer--) & 0x00FF) << 8) + regY;
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << (int)pointer;
	operandString = "($" + ss.str() + "),Y";
	if ((address & 0x00FF) < regY)
		return true;
	return false;
}
bool CPU_6502::IMP()		// Implicit
{
	operandString = "";
	return false;
}
bool CPU_6502::IMM()		// Immediate
{
	address = pc++;
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << (int)bus->Read(address);
	operandString = "#" + ss.str();
	return false;
}
bool CPU_6502::ZP()			// Zero Page
{
	address = bus->Read(pc++);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << address;
	operandString = "$" + ss.str();
	return false;
}
bool CPU_6502::ABS()		// Absolute
{
	address = bus->Read(pc++) | (uint16_t)(bus->Read(pc++) << 8);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(4) << hex << address;
	operandString = "$" + ss.str();
	return false;
}
bool CPU_6502::REL()		// Relative
{
	uint8_t offset = bus->Read(pc++);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(2) << hex << offset;
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
	return false;
}
bool CPU_6502::IND()		// Indirect
{
	uint16_t pointer = bus->Read(pc++) | (uint16_t)(bus->Read(pc++) << 8);
	address = bus->Read(pointer++) | (uint16_t)(bus->Read(pointer) << 8);
	stringstream ss;
	ss << uppercase << setfill('0') << setw(4) << hex << pointer;
	operandString = "($" + ss.str() + ")";
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
		AddCarry(bus->Read(address) ^ 0xFF + 1);		// Subtract is the same as add negative
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
		if (operandString.compare("") != 0)
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
	}
	return false;
}
bool CPU_6502::ROL(bool disassemble)
{
	opCodeString = "ROL";
	if (!disassemble)
	{
		if (operandString.compare("") != 0)
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
	}
	return false;
}
bool CPU_6502::LSR(bool disassemble)
{
	opCodeString = "LSR";
	if (!disassemble)
	{
		if (operandString.compare("") != 0)
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
	}
	return false;
}
bool CPU_6502::ROR(bool disassemble)
{
	opCodeString = "ROR";
	if (!disassemble)
	{
		if (operandString.compare("") != 0)
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
	}
	return false;
}

// Move Commands
bool CPU_6502::LDA(bool disassemble)
{
	opCodeString = "LDA";
	if (!disassemble)
	{
		regA = bus->Read(address);
		SetN(regA & 0x80);
		SetZ(regA == 0);
	}
	return true;
}
bool CPU_6502::STA(bool disassemble)
{
	opCodeString = "STA";
	if (!disassemble)
	{
		bus->Write(address, regA);
	}
	return false;
}
bool CPU_6502::LDX(bool disassemble)
{
	opCodeString = "LDX";
	if (!disassemble)
	{
		regX = bus->Read(address);
		SetN(regX & 0x80);
		SetZ(regX == 0);
	}
	return true;
}
bool CPU_6502::STX(bool disassemble)
{
	opCodeString = "STX";
	if (!disassemble)
	{
		bus->Write(address, regX);
	}
	return false;
}
bool CPU_6502::LDY(bool disassemble)
{
	opCodeString = "LDY";
	if (!disassemble)
	{
		regY = bus->Read(address);
		SetN(regY & 0x80);
		SetZ(regY == 0);
	}
	return true;
}
bool CPU_6502::STY(bool disassemble)
{
	opCodeString = "STY";
	if (!disassemble)
	{
		bus->Write(address, regY);
	}
	return false;
}
bool CPU_6502::TAX(bool disassemble)
{
	opCodeString = "TAX";
	if (!disassemble)
	{
		regX = regA;
		SetN(regX & 0x80);
		SetZ(regX == 0);
	}
	return false;
}
bool CPU_6502::TXA(bool disassemble)
{
	opCodeString = "TXA";
	if (!disassemble)
	{
		regA = regX;
		SetN(regA & 0x80);
		SetZ(regA == 0);
	}
	return false;
}
bool CPU_6502::TAY(bool disassemble)
{
	opCodeString = "TAY";
	if (!disassemble)
	{
		regY = regA;
		SetN(regY & 0x80);
		SetZ(regY == 0);
	}
	return false;
}
bool CPU_6502::TYA(bool disassemble)
{
	opCodeString = "TYA";
	if (!disassemble)
	{
		regA = regY;
		SetN(regA & 0x80);
		SetZ(regA == 0);
	}
	return false;
}
bool CPU_6502::TSX(bool disassemble)
{
	opCodeString = "TSX";
	if (!disassemble)
	{
		regX = sp;
		SetN(regX & 0x80);
		SetZ(regX == 0);
	}
	return false;
}
bool CPU_6502::TXS(bool disassemble)
{
	opCodeString = "TXS";
	if (!disassemble)
	{
		sp = regX;
	}
	return false;
}
bool CPU_6502::PLA(bool disassemble)
{
	opCodeString = "PLA";
	if (!disassemble)
	{
		regA = bus->Read(++sp | 0x0100);
		SetN(regA & 0x80);
		SetZ(regA == 0);
	}
	return false;
}
bool CPU_6502::PHA(bool disassemble)
{
	opCodeString = "PHA";
	if (!disassemble)
	{
		bus->Write(sp-- | 0x0100, regA);
	}
	return false;
}
bool CPU_6502::PLP(bool disassemble)
{
	opCodeString = "PLP";
	if (!disassemble)
	{
		status.p = bus->Read(++sp | 0x0100);
	}
	return false;
}
bool CPU_6502::PHP(bool disassemble)
{
	opCodeString = "PHP";
	if (!disassemble)
	{
		status.B = 0x11;
		bus->Write(sp-- | 0x0100, status.p);
	}
	return false;
}

// Jump/Flag Commands
bool CPU_6502::BPL(bool disassemble)
{
	opCodeString = "BPL";
	if (!disassemble)
	{
		if (!status.N)
			pc = address;
	}
	return true;
}
bool CPU_6502::BMI(bool disassemble)
{
	opCodeString = "BMI";
	if (!disassemble)
	{
		if (status.N)
			pc = address;
	}
	return true;
}
bool CPU_6502::BVC(bool disassemble)
{
	opCodeString = "BVC";
	if (!disassemble)
	{
		if (!status.V)
			pc = address;
	}
	return true;
}
bool CPU_6502::BVS(bool disassemble)
{
	opCodeString = "BVS";
	if (!disassemble)
	{
		if (status.V)
			pc = address;
	}
	return true;
}
bool CPU_6502::BCC(bool disassemble)
{
	opCodeString = "BCC";
	if (!disassemble)
	{
		if (!status.C)
			pc = address;
	}
	return true;
}
bool CPU_6502::BCS(bool disassemble)
{
	opCodeString = "BCS";
	if (!disassemble)
	{
		if (status.C)
			pc = address;
	}
	return true;
}
bool CPU_6502::BNE(bool disassemble)
{
	opCodeString = "BNE";
	if (!disassemble)
	{
		if (!status.Z)
			pc = address;
	}
	return true;
}
bool CPU_6502::BEQ(bool disassemble)
{
	opCodeString = "BEQ";
	if (!disassemble)
	{
		if (status.Z)
			pc = address;
	}
	return true;
}
bool CPU_6502::BRK(bool disassemble)
{
	opCodeString = "BRK";
	if (!disassemble)
	{
		status.B = 0x11;
		bus->Write(sp-- | 0x0100, (pc & 0xFF00) >> 8);
		bus->Write(sp-- | 0x0100, pc & 0x00FF);
		bus->Write(sp-- | 0x0100, status.p);
		pc = bus->Read(0xFFFE) | (bus->Read(0xFFFF) << 8);
		status.I = 1;
	}
	return false;
}
bool CPU_6502::INT(bool disassemble)
{
	opCodeString = "INT";
	if (!disassemble)
	{
		status.B = 0x10;
		bus->Write(sp-- | 0x0100, (pc & 0xFF00) >> 8);
		bus->Write(sp-- | 0x0100, pc & 0x00FF);
		bus->Write(sp-- | 0x0100, status.p);
		pc = bus->Read(address) | (bus->Read(address + 1) << 8);
		status.I = 1;
	}
	return false;
}
bool CPU_6502::RTI(bool disassemble)
{
	opCodeString = "RTI";
	if (!disassemble)
	{
		status.p = bus->Read(++sp | 0x0100);
		pc = bus->Read(++sp | 0x0100) | (bus->Read(++sp | 0x0100) << 8);
	}
	return false;
}
bool CPU_6502::JSR(bool disassemble)
{
	opCodeString = "JSR";
	if (!disassemble)
	{
		bus->Write(sp-- | 0x0100, (pc & 0xFF00) >> 8);
		bus->Write(sp-- | 0x0100, pc & 0x00FF);
		pc = bus->Read(address) | (uint16_t)(bus->Read(address + 1) << 8);
	}
	return false;
}
bool CPU_6502::RTS(bool disassemble)
{
	opCodeString = "RTS";
	if (!disassemble)
	{
		pc = bus->Read(++sp | 0x0100) | (bus->Read(++sp | 0x0100) << 8);
	}
	return false;
}
bool CPU_6502::JMP(bool disassemble)
{
	opCodeString = "JMP";
	if (!disassemble)
	{
		pc = bus->Read(address) | (uint16_t)(bus->Read(address + 1) << 8);
	}
	return false;
}
bool CPU_6502::BIT(bool disassemble)
{
	opCodeString = "BIT";
	if (!disassemble)
	{
		SetN(bus->Read(address) & 0x80);
		SetV(bus->Read(address) & 0x40);
		SetZ(bus->Read(address) & regA);
	}
	return false;
}
bool CPU_6502::CLC(bool disassemble)
{
	opCodeString = "CLC";
	if (!disassemble)
	{
		SetC(0);
	}
	return false;
}
bool CPU_6502::SEC(bool disassemble)
{
	opCodeString = "SEC";
	if (!disassemble)
	{
		SetC(1);
	}
	return false;
}
bool CPU_6502::CLD(bool disassemble)
{
	opCodeString = "CLD";
	if (!disassemble)
	{
		SetD(0);
	}
	return false;
}
bool CPU_6502::SED(bool disassemble)
{
	opCodeString = "SED";
	if (!disassemble)
	{
		SetD(1);
	}
	return false;
}
bool CPU_6502::CLI(bool disassemble)
{
	opCodeString = "CLI";
	if (!disassemble)
	{
		SetI(0);
	}
	return false;
}
bool CPU_6502::SEI(bool disassemble)
{
	opCodeString = "SEI";
	if (!disassemble)
	{
		SetI(1);
	}
	return false;
}
bool CPU_6502::CLV(bool disassemble)
{
	opCodeString = "CLV";
	if (!disassemble)
	{
		SetV(0);
	}
	return false;
}
bool CPU_6502::NOP(bool disassemble)
{
	opCodeString = "NOP";
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
