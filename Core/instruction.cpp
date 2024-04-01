#include "instruction.h"

namespace RV32IM
{
	Instruction::Instruction() : Instruction(0x00000013) {}

	Instruction::Instruction(const inst_data& instruction_data): opcode(RI), rs1(0), rs2(0), rd(0), funct3(), funct7(),
	                                                             immediate(0),
	                                                             inst(instruction_data),
	                                                             type()
	{
	}

	bool Instruction::has_rs1() const
	{
		return type == InstructionFormat::R ||
			   type == InstructionFormat::I ||
			   type == InstructionFormat::S ||
			   type == InstructionFormat::B;
	}

	bool Instruction::has_rs2() const
	{
		return type == InstructionFormat::R ||
			   type == InstructionFormat::S ||
			   type == InstructionFormat::B;
	}

	InstructionR::InstructionR(const inst_data& instruction_data)
	{
		opcode = static_cast<Opcodes>(mask_data(instruction_data, 0, 6));
		rd = mask_data(instruction_data, 7, 11);
		funct3 = static_cast<Funct3>(mask_data(instruction_data, 12, 14));
		rs1 = mask_data(instruction_data, 15, 19);
		rs2 = mask_data(instruction_data, 20, 24);
		funct7 = static_cast<Funct7>(mask_data(instruction_data, 25, 31));
		type = InstructionFormat::R;
		inst = instruction_data;
	}

	InstructionI::InstructionI(const inst_data& instruction_data)
	{
		opcode = static_cast<Opcodes>(mask_data(instruction_data, 0, 6));
		rd = mask_data(instruction_data, 7, 11);
		funct3 = static_cast<Funct3>(mask_data(instruction_data, 12, 14));
		rs1 = mask_data(instruction_data, 15, 19);
		immediate = mask_data(instruction_data, 20, 30) | 
					sign_extend(mask_data(instruction_data, 31, 31), 11);
		type = InstructionFormat::I;
		inst = instruction_data;
	}

	InstructionS::InstructionS(const inst_data& instruction_data)
	{
		opcode = static_cast<Opcodes>(mask_data(instruction_data, 0, 6));
		funct3 = static_cast<Funct3>(mask_data(instruction_data, 12, 14));
		rs1 = mask_data(instruction_data, 15, 19);
		rs2 = mask_data(instruction_data, 20, 24);
		immediate = mask_data(instruction_data, 7, 11) |
					mask_data(instruction_data, 25, 30) << 5 |
					sign_extend(mask_data(instruction_data, 31, 31), 11);
		type = InstructionFormat::S;
		inst = instruction_data;
	}

	InstructionB::InstructionB(const inst_data& instruction_data)
	{
		opcode = static_cast<Opcodes>(mask_data(instruction_data, 0, 6));
		funct3 = static_cast<Funct3>(mask_data(instruction_data, 12, 14));
		rs1 = mask_data(instruction_data, 15, 19);
		rs2 = mask_data(instruction_data, 20, 24);
		immediate = mask_data(instruction_data, 8, 11) << 1 |
					mask_data(instruction_data, 25, 30) << 5 |
					mask_data(instruction_data, 7, 7) << 11 |
					sign_extend(mask_data(instruction_data, 31, 31), 12);
		type = InstructionFormat::B;
		inst = instruction_data;
	}

	InstructionU::InstructionU(const inst_data& instruction_data)
	{
		opcode = static_cast<Opcodes>(mask_data(instruction_data, 0, 6));
		rd = mask_data(instruction_data, 7, 11);
		immediate = mask_data(instruction_data, 12, 19) << 12 |
					mask_data(instruction_data, 20, 30) << 20 |
					mask_data(instruction_data, 31, 31) << 31;
		type = InstructionFormat::U;
		inst = instruction_data;
	}

	InstructionJ::InstructionJ(const inst_data& instruction_data)
	{
		opcode = static_cast<Opcodes>(mask_data(instruction_data, 0, 6));
		rd = mask_data(instruction_data, 7, 11);
		immediate = mask_data(instruction_data, 21, 30) << 1 |
					mask_data(instruction_data, 20, 20) << 11 |
					mask_data(instruction_data, 12, 19) << 12 |
					sign_extend(mask_data(instruction_data, 31, 31), 20);
		type = InstructionFormat::J;
		inst = instruction_data;
	}

	InstructionNOP::InstructionNOP(): InstructionI(0x00000013) {}
}
