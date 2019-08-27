#pragma once
#include <cstdint>

class CPU_6502
{
public:
	CPU_6502();

private:

	struct OpCode
	{
		bool (CPU_6502::*instruction)();
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
	bool ORA();
	bool AND();
	bool EOR();
	bool ADC();
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
	bool SBC();
	bool AHX();
	bool SHY();
	bool SHX();
	bool TAS();
	bool LAS();
	bool KIL();
};

