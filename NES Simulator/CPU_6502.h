#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include "Bus.h"

using namespace std;

class CPU_6502
{
public:
	CPU_6502(Bus *bus);
	~CPU_6502();

	struct DisassembledInstruction
	{
		uint16_t address;
		string instructionString;
		uint8_t instructionSize;
	};

	void Reset();
	bool Clock();
	void Step();
	void IRQ();
	void NMI();
	uint8_t GetA() const;
	uint8_t GetX() const;
	uint8_t GetY() const;
	uint8_t GetStatus() const;
	uint16_t GetProgramCounter() const;
	uint8_t GetStackPointer() const;
	const std::vector<CPU_6502::DisassembledInstruction>* Disassemble(uint16_t startAddress, uint16_t size);


private:

	enum InterruptType { INTERRUPT_NONE, INTERRUPT_IRQ, INTERRUPT_NMI };

	struct OpCode
	{
		bool (CPU_6502::* instruction)();
		bool (CPU_6502::* mode)();
		uint8_t cycles;
		const char* opCode;
		string(CPU_6502::* mode_dis)(uint16_t&);
	};

	using a = CPU_6502;
	OpCode opCodes[0x100] = {
		{&a::BRK, &a::IMP, 7, "BRK", &a::IMP_dis}, {&a::ORA, &a::IZX, 6, "ORA", &a::IZX_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::SLO, &a::IZX, 8, "SLO", &a::IZX_dis}, {&a::NOP, &a::ZP, 3, "NOP", &a::ZP_dis}, {&a::ORA, &a::ZP, 3, "ORA", &a::ZP_dis}, {&a::ASL, &a::ZP, 5, "ASL", &a::ZP_dis}, {&a::SLO, &a::ZP, 5, "SLO", &a::ZP_dis}, {&a::PHP, &a::IMP, 3, "PHP", &a::IMP_dis}, {&a::ORA, &a::IMM, 2, "ORA", &a::IMM_dis}, {&a::ASL, &a::IMP, 2, "ASL", &a::IMP_dis}, {&a::ANC, &a::IMM, 2, "ANC", &a::IMM_dis}, {&a::NOP, &a::ABS, 4, "NOP", &a::ABS_dis}, {&a::ORA, &a::ABS, 4, "ORA", &a::ABS_dis}, {&a::ASL, &a::ABS, 6, "ASL", &a::ABS_dis}, {&a::SLO, &a::ABS, 6, "SLO", &a::ABS_dis},
		{&a::BPL, &a::REL, 2, "BPL", &a::REL_dis}, {&a::ORA, &a::IZY, 5, "ORA", &a::IZY_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::SLO, &a::IZY, 8, "SLO", &a::IZY_dis}, {&a::NOP, &a::ZPX, 4, "NOP", &a::ZPX_dis}, {&a::ORA, &a::ZPX, 4, "ORA", &a::ZPX_dis}, {&a::ASL, &a::ZPX, 6, "ASL", &a::ZPX_dis}, {&a::SLO, &a::ZPX, 6, "SLO", &a::ZPX_dis}, {&a::CLC, &a::IMP, 2, "CLC", &a::IMP_dis}, {&a::ORA, &a::ABY, 4, "ORA", &a::ABY_dis}, {&a::NOP, &a::IMP, 2, "NOP", &a::IMP_dis}, {&a::SLO, &a::ABY, 2, "SLO", &a::ABY_dis}, {&a::NOP, &a::ABX, 4, "NOP", &a::ABX_dis}, {&a::ORA, &a::ABX, 4, "ORA", &a::ABX_dis}, {&a::ASL, &a::ABX, 7, "ASL", &a::ABX_dis}, {&a::SLO, &a::ABX, 7, "SLO", &a::ABX_dis},
		{&a::JSR, &a::ABS, 6, "JSR", &a::ABS_dis}, {&a::AND, &a::IZX, 6, "AND", &a::IZX_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::RLA, &a::IZX, 8, "RLA", &a::IZX_dis}, {&a::BIT, &a::ZP, 3, "BIT", &a::ZP_dis}, {&a::AND, &a::ZP, 3, "AND", &a::ZP_dis}, {&a::ROL, &a::ZP, 5, "ROL", &a::ZP_dis}, {&a::RLA, &a::ZP, 5, "RLA", &a::ZP_dis}, {&a::PLP, &a::IMP, 4, "PLP", &a::IMP_dis}, {&a::AND, &a::IMM, 2, "AND", &a::IMM_dis}, {&a::ROL, &a::IMP, 2, "ROL", &a::IMP_dis}, {&a::ANC, &a::IMM, 2, "ANC", &a::IMM_dis}, {&a::BIT, &a::ABS, 4, "BIT", &a::ABS_dis}, {&a::AND, &a::ABS, 4, "AND", &a::ABS_dis}, {&a::ROL, &a::ABS, 6, "ROL", &a::ABS_dis}, {&a::RLA, &a::ABS, 6, "RLA", &a::ABS_dis},
		{&a::BMI, &a::REL, 2, "BMI", &a::REL_dis}, {&a::AND, &a::IZY, 5, "AND", &a::IZY_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::RLA, &a::IZY, 8, "RLA", &a::IZY_dis}, {&a::NOP, &a::ZPX, 4, "NOP", &a::ZPX_dis}, {&a::AND, &a::ZPX, 4, "AND", &a::ZPX_dis}, {&a::ROL, &a::ZPX, 6, "ROL", &a::ZPX_dis}, {&a::RLA, &a::ZPX, 6, "RLA", &a::ZPX_dis}, {&a::SEC, &a::IMP, 2, "SEC", &a::IMP_dis}, {&a::AND, &a::ABY, 4, "AND", &a::ABY_dis}, {&a::NOP, &a::IMP, 2, "NOP", &a::IMP_dis}, {&a::RLA, &a::ABY, 7, "rla", &a::ABY_dis}, {&a::NOP, &a::ABX, 4, "NOP", &a::ABX_dis}, {&a::AND, &a::ABX, 4, "AND", &a::ABX_dis}, {&a::ROL, &a::ABX, 7, "ROL", &a::ABX_dis}, {&a::RLA, &a::ABX, 7, "RLA", &a::ABX_dis},
		{&a::RTI, &a::IMP, 6, "RTI", &a::IMP_dis}, {&a::EOR, &a::IZX, 6, "EOR", &a::IZX_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::SRE, &a::IZX, 8, "SRE", &a::IZX_dis}, {&a::NOP, &a::ZP, 3, "NOP", &a::ZP_dis}, {&a::EOR, &a::ZP, 3, "EOR", &a::ZP_dis}, {&a::LSR, &a::ZP, 5, "LSR", &a::ZP_dis}, {&a::SRE, &a::ZP, 5, "SRE", &a::ZP_dis}, {&a::PHA, &a::IMP, 3, "PHA", &a::IMP_dis}, {&a::EOR, &a::IMM, 2, "EOR", &a::IMM_dis}, {&a::LSR, &a::IMP, 2, "LSR", &a::IMP_dis}, {&a::ALR, &a::IMM, 2, "ALR", &a::IMM_dis}, {&a::JMP, &a::ABS, 3, "JMP", &a::ABS_dis}, {&a::EOR, &a::ABS, 4, "EOR", &a::ABS_dis}, {&a::LSR, &a::ABS, 6, "LSR", &a::ABS_dis}, {&a::SRE, &a::ABS, 6, "SRE", &a::ABS_dis},
		{&a::BVC, &a::REL, 2, "BVC", &a::REL_dis}, {&a::EOR, &a::IZY, 5, "EOR", &a::IZY_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::SRE, &a::IZY, 8, "SRE", &a::IZY_dis}, {&a::NOP, &a::ZPX, 4, "NOP", &a::ZPX_dis}, {&a::EOR, &a::ZPX, 4, "EOR", &a::ZPX_dis}, {&a::LSR, &a::ZPX, 6, "LSR", &a::ZPX_dis}, {&a::SRE, &a::ZPX, 6, "SRE", &a::ZPX_dis}, {&a::CLI, &a::IMP, 2, "CLI", &a::IMP_dis}, {&a::EOR, &a::ABY, 4, "EOR", &a::ABY_dis}, {&a::NOP, &a::IMP, 2, "NOP", &a::IMP_dis}, {&a::SRE, &a::ABY, 7, "SRE", &a::ABY_dis}, {&a::NOP, &a::ABX, 4, "NOP", &a::ABX_dis}, {&a::EOR, &a::ABX, 4, "EOR", &a::ABX_dis}, {&a::LSR, &a::ABX, 7, "LSR", &a::ABX_dis}, {&a::SRE, &a::ABX, 7, "SRE", &a::ABX_dis},
		{&a::RTS, &a::IMP, 6, "RTS", &a::IMP_dis}, {&a::ADC, &a::IZX, 6, "ADC", &a::IZX_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::RRA, &a::IZX, 8, "RRA", &a::IZX_dis}, {&a::NOP, &a::ZP, 3, "NOP", &a::ZP_dis}, {&a::ADC, &a::ZP, 3, "ADC", &a::ZP_dis}, {&a::ROR, &a::ZP, 5, "ROR", &a::ZP_dis}, {&a::RRA, &a::ZP, 5, "RRA", &a::ZP_dis}, {&a::PLA, &a::IMP, 4, "PLA", &a::IMP_dis}, {&a::ADC, &a::IMM, 2, "ADC", &a::IMM_dis}, {&a::ROR, &a::IMP, 2, "ROR", &a::IMP_dis}, {&a::ARR, &a::IMM, 2, "ARR", &a::IMM_dis}, {&a::JMP, &a::IND, 5, "JMP", &a::IND_dis}, {&a::ADC, &a::ABS, 4, "ADC", &a::ABS_dis}, {&a::ROR, &a::ABS, 6, "ROR", &a::ABS_dis}, {&a::RRA, &a::ABS, 6, "RRA", &a::ABS_dis},
		{&a::BVS, &a::REL, 2, "BVS", &a::REL_dis}, {&a::ADC, &a::IZY, 5, "ADC", &a::IZY_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::RRA, &a::IZY, 8, "RRA", &a::IZY_dis}, {&a::NOP, &a::ZPX, 4, "NOP", &a::ZPX_dis}, {&a::ADC, &a::ZPX, 4, "ADC", &a::ZPX_dis}, {&a::ROR, &a::ZPX, 6, "ROR", &a::ZPX_dis}, {&a::RRA, &a::ZPX, 6, "RRA", &a::ZPX_dis}, {&a::SEI, &a::IMP, 2, "SEI", &a::IMP_dis}, {&a::ADC, &a::ABY, 4, "ADC", &a::ABY_dis}, {&a::NOP, &a::IMP, 2, "NOP", &a::IMP_dis}, {&a::RRA, &a::ABY, 7, "RRA", &a::ABY_dis}, {&a::NOP, &a::ABX, 4, "NOP", &a::ABX_dis}, {&a::ADC, &a::ABX, 4, "ADC", &a::ABX_dis}, {&a::ROR, &a::ABX, 7, "ROR", &a::ABX_dis}, {&a::RRA, &a::ABX, 7, "RRA", &a::ABX_dis},
		{&a::NOP, &a::IMM, 2, "NOP", &a::IMM_dis}, {&a::STA, &a::IZX, 6, "STA", &a::IZX_dis}, {&a::NOP, &a::IMM, 2, "NOP", &a::IMM_dis}, {&a::SAX, &a::IZX, 6, "SAX", &a::IZX_dis}, {&a::STY, &a::ZP, 3, "STY", &a::ZP_dis}, {&a::STA, &a::ZP, 3, "STA", &a::ZP_dis}, {&a::STX, &a::ZP, 3, "STX", &a::ZP_dis}, {&a::SAX, &a::ZP, 3, "SAX", &a::ZP_dis}, {&a::DEY, &a::IMP, 2, "DEY", &a::IMP_dis}, {&a::NOP, &a::IMM, 2, "NOP", &a::IMM_dis}, {&a::TXA, &a::IMP, 2, "TXA", &a::IMP_dis}, {&a::XAA, &a::IMM, 2, "XAA", &a::IMM_dis}, {&a::STY, &a::ABS, 4, "STY", &a::ABS_dis}, {&a::STA, &a::ABS, 4, "STA", &a::ABS_dis}, {&a::STX, &a::ABS, 4, "STX", &a::ABS_dis}, {&a::SAX, &a::ABS, 4, "SAX", &a::ABS_dis},
		{&a::BCC, &a::REL, 2, "BCC", &a::REL_dis}, {&a::STA, &a::IZY, 6, "STA", &a::IZY_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::AHX, &a::IZY, 6, "AHX", &a::IZY_dis}, {&a::STY, &a::ZPX, 4, "STY", &a::ZPX_dis}, {&a::STA, &a::ZPX, 4, "STA", &a::ZPX_dis}, {&a::STX, &a::ZPY, 4, "STX", &a::ZPY_dis}, {&a::SAX, &a::ZPY, 4, "SAX", &a::ZPY_dis}, {&a::TYA, &a::IMP, 2, "TYA", &a::IMP_dis}, {&a::STA, &a::ABY, 5, "STA", &a::ABY_dis}, {&a::TXS, &a::IMP, 2, "TXS", &a::IMP_dis}, {&a::TAS, &a::ABY, 5, "TAS", &a::ABY_dis}, {&a::SHY, &a::ABX, 5, "SHY", &a::ABX_dis}, {&a::STA, &a::ABX, 5, "STA", &a::ABX_dis}, {&a::SHX, &a::ABY, 5, "SHX", &a::ABY_dis}, {&a::AHX, &a::ABY, 5, "AHX", &a::ABY_dis},
		{&a::LDY, &a::IMM, 2, "LDY", &a::IMM_dis}, {&a::LDA, &a::IZX, 6, "LDA", &a::IZX_dis}, {&a::LDX, &a::IMM, 2, "LDX", &a::IMM_dis}, {&a::LAX, &a::IZX, 6, "LAX", &a::IZX_dis}, {&a::LDY, &a::ZP, 3, "LDY", &a::ZP_dis}, {&a::LDA, &a::ZP, 3, "LDA", &a::ZP_dis}, {&a::LDX, &a::ZP, 3, "LDX", &a::ZP_dis}, {&a::LAX, &a::ZP, 3, "LAX", &a::ZP_dis}, {&a::TAY, &a::IMP, 2, "TAY", &a::IMP_dis}, {&a::LDA, &a::IMM, 2, "LDA", &a::IMM_dis}, {&a::TAX, &a::IMP, 2, "TAX", &a::IMP_dis}, {&a::LAX, &a::IMM, 2, "LAX", &a::IMM_dis}, {&a::LDY, &a::ABS, 4, "LDY", &a::ABS_dis}, {&a::LDA, &a::ABS, 4, "LDA", &a::ABS_dis}, {&a::LDX, &a::ABS, 4, "LDX", &a::ABS_dis}, {&a::LAX, &a::ABS, 4, "LAX", &a::ABS_dis},
		{&a::BCS, &a::REL, 2, "BCS", &a::REL_dis}, {&a::LDA, &a::IZY, 5, "LDA", &a::IZY_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::LAX, &a::IZY, 5, "LAX", &a::IZY_dis}, {&a::LDY, &a::ZPX, 4, "LDY", &a::ZPX_dis}, {&a::LDA, &a::ZPX, 4, "LDA", &a::ZPX_dis}, {&a::LDX, &a::ZPY, 4, "LDX", &a::ZPY_dis}, {&a::LAX, &a::ZPY, 4, "LAX", &a::ZPY_dis}, {&a::CLV, &a::IMP, 2, "CLV", &a::IMP_dis}, {&a::LDA, &a::ABY, 4, "LDA", &a::ABY_dis}, {&a::TSX, &a::IMP, 2, "TSX", &a::IMP_dis}, {&a::LAS, &a::ABY, 4, "LAS", &a::ABY_dis}, {&a::LDY, &a::ABX, 4, "LDY", &a::ABX_dis}, {&a::LDA, &a::ABX, 4, "LDA", &a::ABX_dis}, {&a::LDX, &a::ABY, 4, "LDX", &a::ABY_dis}, {&a::LAX, &a::ABY, 4, "LAX", &a::ABY_dis},
		{&a::CPY, &a::IMM, 2, "CPY", &a::IMM_dis}, {&a::CMP, &a::IZX, 6, "CMP", &a::IZX_dis}, {&a::NOP, &a::IMM, 2, "NOP", &a::IMM_dis}, {&a::DCP, &a::IZX, 8, "DCP", &a::IZX_dis}, {&a::CPY, &a::ZP, 3, "CPY", &a::ZP_dis}, {&a::CMP, &a::ZP, 3, "CMP", &a::ZP_dis}, {&a::DEC, &a::ZP, 5, "DEC", &a::ZP_dis}, {&a::DCP, &a::ZP, 5, "DCP", &a::ZP_dis}, {&a::INY, &a::IMP, 2, "INY", &a::IMP_dis}, {&a::CMP, &a::IMM, 2, "CMP", &a::IMM_dis}, {&a::DEX, &a::IMP, 2, "DEX", &a::IMP_dis}, {&a::AXS, &a::IMM, 2, "AXS", &a::IMM_dis}, {&a::CPY, &a::ABS, 4, "CPY", &a::ABS_dis}, {&a::CMP, &a::ABS, 4, "CMP", &a::ABS_dis}, {&a::DEC, &a::ABS, 6, "DEC", &a::ABS_dis}, {&a::DCP, &a::ABS, 6, "DCP", &a::ABS_dis},
		{&a::BNE, &a::REL, 2, "BNE", &a::REL_dis}, {&a::CMP, &a::IZY, 5, "CMP", &a::IZY_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::DCP, &a::IZY, 8, "DCP", &a::IZY_dis}, {&a::NOP, &a::ZPX, 4, "NOP", &a::ZPX_dis}, {&a::CMP, &a::ZPX, 4, "CMP", &a::ZPX_dis}, {&a::DEC, &a::ZPX, 6, "DEC", &a::ZPX_dis}, {&a::DCP, &a::ZPX, 6, "DCP", &a::ZPX_dis}, {&a::CLD, &a::IMP, 2, "CLD", &a::IMP_dis}, {&a::CMP, &a::ABY, 4, "CMP", &a::ABY_dis}, {&a::NOP, &a::IMP, 2, "NOP", &a::IMP_dis}, {&a::DCP, &a::ABY, 7, "DCP", &a::ABY_dis}, {&a::NOP, &a::ABX, 4, "NOP", &a::ABX_dis}, {&a::CMP, &a::ABX, 4, "CMP", &a::ABX_dis}, {&a::DEC, &a::ABX, 7, "DEC", &a::ABX_dis}, {&a::DCP, &a::ABX, 7, "DCP", &a::ABX_dis},
		{&a::CPX, &a::IMM, 2, "CPX", &a::IMM_dis}, {&a::SBC, &a::IZX, 6, "SBC", &a::IZX_dis}, {&a::NOP, &a::IMM, 2, "NOP", &a::IMM_dis}, {&a::ISC, &a::IZX, 8, "ISC", &a::IZX_dis}, {&a::CPX, &a::ZP, 3, "CPX", &a::ZP_dis}, {&a::SBC, &a::ZP, 3, "SBC", &a::ZP_dis}, {&a::INC, &a::ZP, 5, "INC", &a::ZP_dis}, {&a::ISC, &a::ZP, 5, "ISC", &a::ZP_dis}, {&a::INX, &a::IMP, 2, "INX", &a::IMP_dis}, {&a::SBC, &a::IMM, 2, "SBC", &a::IMM_dis}, {&a::NOP, &a::IMP, 2, "NOP", &a::IMP_dis}, {&a::SBC, &a::IMM, 2, "SBC", &a::IMM_dis}, {&a::CPX, &a::ABS, 4, "CPX", &a::ABS_dis}, {&a::SBC, &a::ABS, 4, "SBC", &a::ABS_dis}, {&a::INC, &a::ABS, 6, "INC", &a::ABS_dis}, {&a::ISC, &a::ABS, 6, "ISC", &a::ABS_dis},
		{&a::BEQ, &a::REL, 2, "BEQ", &a::REL_dis}, {&a::SBC, &a::IZY, 5, "SBC", &a::IZY_dis}, {&a::KIL, &a::IMP, 1, "KIL", &a::IMP_dis}, {&a::ISC, &a::IZY, 8, "ISC", &a::IZY_dis}, {&a::NOP, &a::ZPX, 4, "NOP", &a::ZPX_dis}, {&a::SBC, &a::ZPX, 4, "SBC", &a::ZPX_dis}, {&a::INC, &a::ZPX, 6, "INC", &a::ZPX_dis}, {&a::ISC, &a::ZPX, 6, "ISC", &a::ZPX_dis}, {&a::SED, &a::IMP, 2, "SED", &a::IMP_dis}, {&a::SBC, &a::ABY, 4, "SBC", &a::ABY_dis}, {&a::NOP, &a::IMP, 2, "NOP", &a::IMP_dis}, {&a::ISC, &a::ABY, 7, "ISC", &a::ABY_dis}, {&a::NOP, &a::ABX, 4, "NOP", &a::ABX_dis}, {&a::SBC, &a::ABX, 4, "SBC", &a::ABX_dis}, {&a::INC, &a::ABX, 7, "INC", &a::ABX_dis}, {&a::ISC, &a::ABX, 7, "ISC", &a::ABX_dis}
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
	bool bImplied;
	uint16_t address;
	uint8_t data;
	Bus* bus;
	InterruptType currentInterrupt;
	std::vector<DisassembledInstruction> disassembleInfo;
	std::map<uint16_t, int> instructionMap;


	// ****************
	// Addressing Modes
	// ****************
	bool ZPX();		// Zero Page Indexed (X)
	string ZPX_dis(uint16_t& counter);
	bool ZPY();		// Zero Page Indexed (Y)
	string ZPY_dis(uint16_t& counter);
	bool ABX();		// Absolute Indexed (X)
	string ABX_dis(uint16_t& counter);
	bool ABY();		// Absolute Indexed (Y)
	string ABY_dis(uint16_t& counter);
	bool IZX();		// Indexed Indirect
	string IZX_dis(uint16_t& counter);
	bool IZY();		// Indirect Indexed
	string IZY_dis(uint16_t& counter);
	bool IMP();		// Implicit
	string IMP_dis(uint16_t& counter);
	bool IMM();		// Immediate
	string IMM_dis(uint16_t& counter);
	bool ZP();		// Zero Page
	string ZP_dis(uint16_t& counter);
	bool ABS();		// Absolute
	string ABS_dis(uint16_t& counter);
	bool REL();		// Relative
	string REL_dis(uint16_t& counter);
	bool IND();		// Indirect
	string IND_dis(uint16_t& counter);


	// ************
	// Instructions
	// ************
	
	// Logical and Arithmetic Commands
	bool ORA();
	bool AND();
	bool EOR();
	bool ADC();
	bool SBC();
	bool CMP();
	bool CPX();
	bool CPY();
	bool DEC();
	bool DEX();
	bool DEY();
	bool INC();
	bool INX();
	bool INY();
	bool ASL();
	bool ROL();
	bool LSR();
	bool ROR();

	// Move Commands
	bool LDA();
	bool STA();
	bool LDX();
	bool STX();
	bool LDY();
	bool STY();
	bool TAX();
	bool TXA();
	bool TAY();
	bool TYA();
	bool TSX();
	bool TXS();
	bool PLA();
	bool PHA();
	bool PLP();
	bool PHP();

	// Jump/Flag Commands
	bool BPL();
	bool BMI();
	bool BVC();
	bool BVS();
	bool BCC();
	bool BCS();
	bool BNE();
	bool BEQ();
	bool BRK();
	bool INT();
	bool RTI();
	bool JSR();
	bool RTS();
	bool JMP();
	bool BIT();
	bool CLC();
	bool SEC();
	bool CLD();
	bool SED();
	bool CLI();
	bool SEI();
	bool CLV();
	bool NOP();

	// Illegal Opcodes
	bool SLO();
	bool RLA();
	bool SRE();
	bool RRA();
	bool SAX();
	bool LAX();
	bool DCP();
	bool ISC();
	bool ANC();
	bool ALR();
	bool ARR();
	bool XAA();
	bool AXS();
	bool AHX();
	bool SHY();
	bool SHX();
	bool TAS();
	bool LAS();
	bool KIL();

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

