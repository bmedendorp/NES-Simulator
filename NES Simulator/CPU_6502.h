#pragma once
#include <cstdint>
#include <string>
#include "Memory.h"

using namespace std;

class CPU_6502
{
public:
	CPU_6502(Memory *bus, uint8_t disassembleLines = 13);
	~CPU_6502();

	void Reset();
	bool Clock();
	void Step();
	void IRQ();
	uint8_t GetA() const;
	uint8_t GetX() const;
	uint8_t GetY() const;
	uint8_t GetStatus() const;
	uint16_t GetProgramCounter() const;
	uint8_t GetStackPointer() const;

	struct DisassembledInstruction
	{
		uint16_t address;
		string instructionString;
		uint8_t instructionSize;
	};

	struct DisassembleInfo
	{
		uint8_t count;
		DisassembledInstruction* instructions;
	};

	uint8_t Disassemble(uint16_t programStart, uint8_t instructionCount, CPU_6502::DisassembleInfo* data, uint16_t maxBytes = 0);
	const CPU_6502::DisassembleInfo* GetDisassembleInfo() const;

private:

	struct OpCode
	{
		bool (CPU_6502::*instruction)(bool);
		bool (CPU_6502::*mode)();
		uint8_t cycles;
	};

	using a = CPU_6502;
	OpCode opCodes[0x100] = {
		{&a::BRK, &a::IMP, 7}, {&a::ORA, &a::IZX, 6}, {&a::KIL, &a::IMP, 1}, {&a::SLO, &a::IZX, 8}, {&a::NOP, &a::ZP, 3}, {&a::ORA, &a::ZP, 3}, {&a::ASL, &a::ZP, 5}, {&a::SLO,&a::ZP, 5}, {&a::PHP, &a::IMP, 3}, {&a::ORA, &a::IMM, 2}, {&a::ASL, &a::IMP, 2}, {&a::ANC, &a::IMM, 2}, {&a::NOP, &a::ABS, 4}, {&a::ORA, &a::ABS, 4}, {&a::ASL, &a::ABS, 6}, {&a::SLO, &a::ABS, 6},
		{&a::BPL, &a::REL, 2}, {&a::ORA, &a::IZY, 5}, {&a::KIL, &a::IMP, 1}, {&a::SLO, &a::IZY, 8}, {&a::NOP, &a::ZPX, 4}, {&a::ORA, &a::ZPX, 4}, {&a::ASL, &a::ZPX, 6}, {&a::SLO, &a::ZPX, 6}, {&a::CLC, &a::IMP, 2}, {&a::ORA, &a::ABY, 4}, {&a::NOP, &a::IMP, 2}, {&a::SLO, &a::ABY, 2}, {&a::NOP, &a::ABX, 4}, {&a::ORA, &a::ABX, 4}, {&a::ASL, &a::ABX, 7}, {&a::SLO, &a::ABX, 7},
		{&a::JSR, &a::ABS, 6}, {&a::AND, &a::IZX, 6}, {&a::KIL, &a::IMP, 1}, {&a::RLA, &a::IZX, 8}, {&a::BIT, &a::ZP, 3}, {&a::AND, &a::ZP, 3}, {&a::ROL, &a::ZP, 5}, {&a::RLA, &a::ZP, 5}, {&a::PLP, &a::IMP, 4}, {&a::AND, &a::IMM, 2}, {&a::ROL, &a::IMP, 2}, {&a::ANC, &a::IMM, 2}, {&a::BIT, &a::ABS, 4}, {&a::AND, &a::ABS, 4}, {&a::ROL, &a::ABS, 6}, {&a::RLA, &a::ABS, 6},
		{&a::BMI, &a::REL, 2}, {&a::AND, &a::IZY, 5}, {&a::KIL, &a::IMP, 1}, {&a::RLA, &a::IZY, 8}, {&a::NOP, &a::ZPX, 4}, {&a::AND, &a::ZPX, 4}, {&a::ROL, &a::ZPX, 6}, {&a::RLA, &a::ZPX, 6}, {&a::SEC, &a::IMP, 2}, {&a::AND, &a::ABY, 4}, {&a::NOP, &a::IMP, 2}, {&a::RLA, &a::ABY, 7}, {&a::NOP, &a::ABX, 4}, {&a::AND, &a::ABX, 4}, {&a::ROL, &a::ABX, 7}, {&a::RLA, &a::ABX, 7},
		{&a::RTI, &a::IMP, 6}, {&a::EOR, &a::IZX, 6}, {&a::KIL, &a::IMP, 1}, {&a::SRE, &a::IZX, 8}, {&a::NOP, &a::ZP, 3}, {&a::EOR, &a::ZP, 3}, {&a::LSR, &a::ZP, 5}, {&a::SRE, &a::ZP, 5}, {&a::PHA, &a::IMP, 3}, {&a::EOR, &a::IMM, 2}, {&a::LSR, &a::IMP, 2}, {&a::ALR, &a::IMM, 2}, {&a::JMP, &a::ABS, 3}, {&a::EOR, &a::ABS, 4}, {&a::LSR, &a::ABS, 6}, {&a::SRE, &a::ABS, 6},
		{&a::BVC, &a::REL, 2}, {&a::EOR, &a::IZY, 5}, {&a::KIL, &a::IMP, 1}, {&a::SRE, &a::IZY, 8}, {&a::NOP, &a::ZPX, 4}, {&a::EOR, &a::ZPX, 4}, {&a::LSR, &a::ZPX, 6}, {&a::SRE, &a::ZPX, 6}, {&a::CLI, &a::IMP, 2}, {&a::EOR, &a::ABY, 4}, {&a::NOP, &a::IMP, 2}, {&a::SRE, &a::ABY, 7}, {&a::NOP, &a::ABX, 4}, {&a::EOR, &a::ABX, 4}, {&a::LSR, &a::ABX, 7}, {&a::SRE, &a::ABX, 7},
		{&a::RTS, &a::IMP, 6}, {&a::ADC, &a::IZX, 6}, {&a::KIL, &a::IMP, 1}, {&a::RRA, &a::IZX, 8}, {&a::NOP, &a::ZP, 3}, {&a::ADC, &a::ZP, 3}, {&a::ROR, &a::ZP, 5}, {&a::RRA, &a::ZP, 5}, {&a::PLA, &a::IMP, 4}, {&a::ADC, &a::IMM, 2}, {&a::ROR, &a::IMP, 2}, {&a::ARR, &a::IMM, 2}, {&a::JMP, &a::IND, 5}, {&a::ADC, &a::ABS, 4}, {&a::ROR, &a::ABS, 6}, {&a::RRA, &a::ABS, 6},
		{&a::BVS, &a::REL, 2}, {&a::ADC, &a::IZY, 5}, {&a::KIL, &a::IMP, 1}, {&a::RRA, &a::IZY, 8}, {&a::NOP, &a::ZPX, 4}, {&a::ADC, &a::ZPX, 4}, {&a::ROR, &a::ZPX, 6}, {&a::RRA, &a::ZPX, 6}, {&a::SEI, &a::IMP, 2}, {&a::ADC, &a::ABY, 4}, {&a::NOP, &a::IMP, 2}, {&a::RRA, &a::ABY, 7}, {&a::NOP, &a::ABX, 4}, {&a::ADC, &a::ABX, 4}, {&a::ROR, &a::ABX, 7}, {&a::RRA, &a::ABX, 7},
		{&a::NOP, &a::IMM, 2}, {&a::STA, &a::IZX, 6}, {&a::NOP, &a::IMM, 2}, {&a::SAX, &a::IZX, 6}, {&a::STY, &a::ZP, 3}, {&a::STA, &a::ZP, 3}, {&a::STX, &a::ZP, 3}, {&a::SAX, &a::ZP, 3}, {&a::DEY, &a::IMP, 2}, {&a::NOP, &a::IMM, 2}, {&a::TXA, &a::IMP, 2}, {&a::XAA, &a::IMM, 2}, {&a::STY, &a::ABS, 4}, {&a::STA, &a::ABS, 4}, {&a::STX, &a::ABS, 4}, {&a::SAX, &a::ABS, 4},
		{&a::BCC, &a::REL, 2}, {&a::STA, &a::IZY, 6}, {&a::KIL, &a::IMP, 1}, {&a::AHX, &a::IZY, 6}, {&a::STY, &a::ZPX, 4}, {&a::STA, &a::ZPX, 4}, {&a::STX, &a::ZPY, 4}, {&a::SAX, &a::ZPY, 4}, {&a::TYA, &a::IMP, 2}, {&a::STA, &a::ABY, 5}, {&a::TXS, &a::IMP, 2}, {&a::TAS, &a::ABY, 5}, {&a::SHY, &a::ABX, 5}, {&a::STA, &a::ABX, 5}, {&a::SHX, &a::ABY, 5}, {&a::AHX, &a::ABY, 5},
		{&a::LDY, &a::IMM, 2}, {&a::LDA, &a::IZX, 6}, {&a::LDX, &a::IMM, 2}, {&a::LAX, &a::IZX, 6}, {&a::LDY, &a::ZP, 3}, {&a::LDA, &a::ZP, 3}, {&a::LDX, &a::ZP, 3}, {&a::LAX, &a::ZP, 3}, {&a::TAY, &a::IMP, 2}, {&a::LDA, &a::IMM, 2}, {&a::TAX, &a::IMP, 2}, {&a::LAX, &a::IMM, 2}, {&a::LDY, &a::ABS, 4}, {&a::LDA, &a::ABS, 4}, {&a::LDX, &a::ABS, 4}, {&a::LAX, &a::ABS, 4},
		{&a::BCS, &a::REL, 2}, {&a::LDA, &a::IZY, 5}, {&a::KIL, &a::IMP, 1}, {&a::LAX, &a::IZY, 5}, {&a::LDY, &a::ZPX, 4}, {&a::LDA, &a::ZPX, 4}, {&a::LDX, &a::ZPY, 4}, {&a::LAX, &a::ZPY, 4}, {&a::CLV, &a::IMP, 2}, {&a::LDA, &a::ABY, 4}, {&a::TSX, &a::IMP, 2}, {&a::LAS, &a::ABY, 4}, {&a::LDY, &a::ABX, 4}, {&a::LDA, &a::ABX, 4}, {&a::LDX, &a::ABY, 4}, {&a::LAX, &a::ABY, 4},
		{&a::CPY, &a::IMM, 2}, {&a::CMP, &a::IZX, 6}, {&a::NOP, &a::IMM, 2}, {&a::DCP, &a::IZX, 8}, {&a::CPY, &a::ZP, 3}, {&a::CMP, &a::ZP, 3}, {&a::DEC, &a::ZP, 5}, {&a::DCP, &a::ZP, 5}, {&a::INY, &a::IMP, 2}, {&a::CMP, &a::IMM, 2}, {&a::DEX, &a::IMP, 2}, {&a::AXS, &a::IMM, 2}, {&a::CPY, &a::ABS, 4}, {&a::CMP, &a::ABS, 4}, {&a::DEC, &a::ABS, 6}, {&a::DCP, &a::ABS, 6},
		{&a::BNE, &a::REL, 2}, {&a::CMP, &a::IZY, 5}, {&a::KIL, &a::IMP, 1}, {&a::DCP, &a::IZY, 8}, {&a::NOP, &a::ZPX, 4}, {&a::CMP, &a::ZPX, 4}, {&a::DEC, &a::ZPX, 6}, {&a::DCP, &a::ZPX, 6}, {&a::CLD, &a::IMP, 2}, {&a::CMP, &a::ABY, 4}, {&a::NOP, &a::IMP, 2}, {&a::DCP, &a::ABY, 7}, {&a::NOP, &a::ABX, 4}, {&a::CMP, &a::ABX, 4}, {&a::DEC, &a::ABX, 7}, {&a::DCP, &a::ABX, 7},
		{&a::CPX, &a::IMM, 2}, {&a::SBC, &a::IZX, 6}, {&a::NOP, &a::IMM, 2}, {&a::ISC, &a::IZX, 8}, {&a::CPX, &a::ZP, 3}, {&a::SBC, &a::ZP, 3}, {&a::INC, &a::ZP, 5}, {&a::ISC, &a::ZP, 5}, {&a::INX, &a::IMP, 2}, {&a::SBC, &a::IMM, 2}, {&a::NOP, &a::IMP, 2}, {&a::SBC, &a::IMM, 2}, {&a::CPX, &a::ABS, 4}, {&a::SBC, &a::ABS, 4}, {&a::INC, &a::ABS, 6}, {&a::ISC, &a::ABS, 6},
		{&a::BEQ, &a::REL, 2}, {&a::SBC, &a::IZY, 5}, {&a::KIL, &a::IMP, 1}, {&a::ISC, &a::IZY, 8}, {&a::NOP, &a::ZPX, 4}, {&a::SBC, &a::ZPX, 4}, {&a::INC, &a::ZPX, 6}, {&a::ISC, &a::ZPX, 6}, {&a::SED, &a::IMP, 2}, {&a::SBC, &a::ABY, 4}, {&a::NOP, &a::IMP, 2}, {&a::ISC, &a::ABY, 7}, {&a::NOP, &a::ABX, 4}, {&a::SBC, &a::ABX, 4}, {&a::INC, &a::ABX, 7}, {&a::ISC, &a::ABX, 7}
	};


	// *********
	// Registers
	// *********
	uint8_t regA;	// Accumulator
	uint8_t regX;	// X Register
	uint8_t regY;	// Y Register
	uint16_t pc;	// Program Counter
	uint8_t sp;		// Stack Pointer
	union {
		uint8_t p;			// Entire Register
		struct {
			uint8_t C : 1;	// Carry Flag
			uint8_t Z : 1;	// Zero Flag
			uint8_t I : 1;	// Interrupt Disable
			uint8_t D : 1;	// Decimal Flag
			uint8_t B : 2;	// "B" Flag
			uint8_t V : 1;	// Overflot Flag
			uint8_t N : 1;	// Negative Flag
		};
	} status;			// Status Register

	
	// Class Globals
	uint16_t address;
	uint8_t data;
	Memory* bus;
	std::string opCodeString;
	std::string operandString;

	// Disassembler Variables
	uint8_t maxDisassemblySize;
	uint8_t disassemblyIndex;
	DisassembleInfo disassembleInfo;

	void AdvanceDisassembler();


	// ****************
	// Addressing Modes
	// ****************
	bool ZPX();		// Zero Page Indexed (X)
	bool ZPY();		// Zero Page Indexed (Y)
	bool ABX();		// Absolute Indexed (X)
	bool ABY();		// Absolute Indexed (Y)
	bool IZX();		// Indexed Indirect
	bool IZY();		// Indirect Indexed
	bool IMP();		// Implicit
	bool IMM();		// Immediate
	bool ZP();		// Zero Page
	bool ABS();		// Absolute
	bool REL();		// Relative
	bool IND();		// Indirect


	// ************
	// Instructions
	// ************
	
	// Logical and Arithmetic Commands
	bool ORA(bool disassemble = false);
	bool AND(bool disassemble = false);
	bool EOR(bool disassemble = false);
	bool ADC(bool disassemble = false);
	bool SBC(bool disassemble = false);
	bool CMP(bool disassemble = false);
	bool CPX(bool disassemble = false);
	bool CPY(bool disassemble = false);
	bool DEC(bool disassemble = false);
	bool DEX(bool disassemble = false);
	bool DEY(bool disassemble = false);
	bool INC(bool disassemble = false);
	bool INX(bool disassemble = false);
	bool INY(bool disassemble = false);
	bool ASL(bool disassemble = false);
	bool ROL(bool disassemble = false);
	bool LSR(bool disassemble = false);
	bool ROR(bool disassemble = false);

	// Move Commands
	bool LDA(bool disassemble = false);
	bool STA(bool disassemble = false);
	bool LDX(bool disassemble = false);
	bool STX(bool disassemble = false);
	bool LDY(bool disassemble = false);
	bool STY(bool disassemble = false);
	bool TAX(bool disassemble = false);
	bool TXA(bool disassemble = false);
	bool TAY(bool disassemble = false);
	bool TYA(bool disassemble = false);
	bool TSX(bool disassemble = false);
	bool TXS(bool disassemble = false);
	bool PLA(bool disassemble = false);
	bool PHA(bool disassemble = false);
	bool PLP(bool disassemble = false);
	bool PHP(bool disassemble = false);

	// Jump/Flag Commands
	bool BPL(bool disassemble = false);
	bool BMI(bool disassemble = false);
	bool BVC(bool disassemble = false);
	bool BVS(bool disassemble = false);
	bool BCC(bool disassemble = false);
	bool BCS(bool disassemble = false);
	bool BNE(bool disassemble = false);
	bool BEQ(bool disassemble = false);
	bool BRK(bool disassemble = false);
	bool RTI(bool disassemble = false);
	bool JSR(bool disassemble = false);
	bool RTS(bool disassemble = false);
	bool JMP(bool disassemble = false);
	bool BIT(bool disassemble = false);
	bool CLC(bool disassemble = false);
	bool SEC(bool disassemble = false);
	bool CLD(bool disassemble = false);
	bool SED(bool disassemble = false);
	bool CLI(bool disassemble = false);
	bool SEI(bool disassemble = false);
	bool CLV(bool disassemble = false);
	bool NOP(bool disassemble = false);

	// Illegal Opcodes
	bool SLO(bool disassemble = false);
	bool RLA(bool disassemble = false);
	bool SRE(bool disassemble = false);
	bool RRA(bool disassemble = false);
	bool SAX(bool disassemble = false);
	bool LAX(bool disassemble = false);
	bool DCP(bool disassemble = false);
	bool ISC(bool disassemble = false);
	bool ANC(bool disassemble = false);
	bool ALR(bool disassemble = false);
	bool ARR(bool disassemble = false);
	bool XAA(bool disassemble = false);
	bool AXS(bool disassemble = false);
	bool AHX(bool disassemble = false);
	bool SHY(bool disassemble = false);
	bool SHX(bool disassemble = false);
	bool TAS(bool disassemble = false);
	bool LAS(bool disassemble = false);
	bool KIL(bool disassemble = false);

	void AddCarry(uint8_t data);
	void Compare(uint8_t a, uint8_t b);
	uint8_t Decrement(uint8_t a);
	uint8_t Increment(uint8_t a);

	// ***************
	// Status Register
	// ***************
	bool SetN(bool n);
	bool SetV(bool v);
	bool SetD(bool d);
	bool SetI(bool i);
	bool SetZ(bool z);
	bool SetC(bool c);
};

