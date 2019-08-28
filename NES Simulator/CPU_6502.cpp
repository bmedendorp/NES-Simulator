#include "CPU_6502.h"

CPU_6502::CPU_6502(Memory* bus)
{
	this->bus = bus;
}

// ****************
// Addressing Modes
// ****************
bool CPU_6502::ZPX()		// Zero Page Indexed (X)
{
	if (bus)
	{
		address = (bus->Read(pc++) + regX) && 0x00FF;
	}
	return false;
}
bool CPU_6502::ZPY()		// Zero Page Indexed (Y)
{
	if (bus)
	{
		address = (bus->Read(pc++) + regY) && 0x00FF;
	}
	return false;
}
bool CPU_6502::ABX()		// Absolute Indexed (X)
{
	if (bus)
	{
		address = (uint16_t)(bus->Read(pc++) + regX) | (uint16_t)bus->Read(pc++) << 8;
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
	}
	return false;
}
bool CPU_6502::IZY()		// Indirect Indexed
{
	if (bus)
	{
		uint8_t pointer = bus->Read(pc++);
		address = (bus->Read(pointer++) | (uint16_t)(bus->Read(pointer) & 0x00FF) << 8) + regY;
		if ((address & 0x00FF) < regY)
			return true;
	}
	return false;
}
bool CPU_6502::IMP()		// Implicit
{
	return false;
}
bool CPU_6502::IMM()		// Immediate
{
	address = pc++;
	return false;
}
bool CPU_6502::ZP()			// Zero Page
{
	if (bus)
	{
		address = bus->Read(pc++);
	}
	return false;
}
bool CPU_6502::ABS()		// Absolute
{
	if (bus)
	{
		address = bus->Read(pc++) | (uint16_t)(bus->Read(pc++) << 8);
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
	}
	return false;
}


// ************
// Instructions
// ************

// Logical and Arithmetic Commands
bool CPU_6502::ORA()
{
	return false;
}
bool CPU_6502::AND()
{
	return false;
}
bool CPU_6502::EOR()
{
	return false;
}
bool CPU_6502::ADC()
{
	return false;
}
bool CPU_6502::CMP()
{
	return false;
}
bool CPU_6502::CPX()
{
	return false;
}
bool CPU_6502::CPY()
{
	return false;
}
bool CPU_6502::DEC()
{
	return false;
}
bool CPU_6502::DEX()
{
	return false;
}
bool CPU_6502::DEY()
{
	return false;
}
bool CPU_6502::INC()
{
	return false;
}
bool CPU_6502::INX()
{
	return false;
}
bool CPU_6502::INY()
{
	return false;
}
bool CPU_6502::ASL()
{
	return false;
}
bool CPU_6502::ROL()
{
	return false;
}
bool CPU_6502::LSR()
{
	return false;
}
bool CPU_6502::ROR()
{
	return false;
}

// Move Commands
bool CPU_6502::LDA()
{
	return false;
}
bool CPU_6502::STA()
{
	return false;
}
bool CPU_6502::LDX()
{
	return false;
}
bool CPU_6502::STX()
{
	return false;
}
bool CPU_6502::LDY()
{
	return false;
}
bool CPU_6502::STY()
{
	return false;
}
bool CPU_6502::TAX()
{
	return false;
}
bool CPU_6502::TXA()
{
	return false;
}
bool CPU_6502::TAY()
{
	return false;
}
bool CPU_6502::TYA()
{
	return false;
}
bool CPU_6502::TSX()
{
	return false;
}
bool CPU_6502::TXS()
{
	return false;
}
bool CPU_6502::PLA()
{
	return false;
}
bool CPU_6502::PHA()
{
	return false;
}
bool CPU_6502::PLP()
{
	return false;
}
bool CPU_6502::PHP()
{
	return false;
}

// Jump/Flag Commands
bool CPU_6502::BPL()
{
	return false;
}
bool CPU_6502::BMI()
{
	return false;
}
bool CPU_6502::BVC()
{
	return false;
}
bool CPU_6502::BVS()
{
	return false;
}
bool CPU_6502::BCC()
{
	return false;
}
bool CPU_6502::BCS()
{
	return false;
}
bool CPU_6502::BNE()
{
	return false;
}
bool CPU_6502::BEQ()
{
	return false;
}
bool CPU_6502::BRK()
{
	return false;
}
bool CPU_6502::RTI()
{
	return false;
}
bool CPU_6502::JSR()
{
	return false;
}
bool CPU_6502::RTS()
{
	return false;
}
bool CPU_6502::JMP()
{
	return false;
}
bool CPU_6502::BIT()
{
	return false;
}
bool CPU_6502::CLC()
{
	return false;
}
bool CPU_6502::SEC()
{
	return false;
}
bool CPU_6502::CLD()
{
	return false;
}
bool CPU_6502::SED()
{
	return false;
}
bool CPU_6502::CLI()
{
	return false;
}
bool CPU_6502::SEI()
{
	return false;
}
bool CPU_6502::CLV()
{
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
bool CPU_6502::SBC()
{
	return false;
}
bool CPU_6502::AHX()
{
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
