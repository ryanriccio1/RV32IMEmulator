#pragma once
#include "common.h"

namespace RV32IM
{
	struct Instruction
	{
		Instruction();
		Instruction(const inst_data& instruction_data);
		Opcodes opcode;
		unsigned_data rs1;
		unsigned_data rs2;
		unsigned_data rd;
		Funct3 funct3;
		Funct7 funct7;
		unsigned_data immediate;
		inst_data inst;
		InstructionFormat type;

		bool has_rs1() const;
		bool has_rs2() const;
	};

	struct InstructionR : Instruction
	{
		InstructionR(const inst_data& instruction_data);
	};
	struct InstructionI : Instruction
	{
		InstructionI(const inst_data& instruction_data);
	};
	struct InstructionS : Instruction
	{
		InstructionS(const inst_data& instruction_data);
	};
	struct InstructionB : Instruction
	{
		InstructionB(const inst_data& instruction_data);
	};
	struct InstructionU : Instruction
	{
		InstructionU(const inst_data& instruction_data);
	};
	struct InstructionJ : Instruction
	{
		InstructionJ(const inst_data& instruction_data);
	};
	struct InstructionNOP : InstructionI
	{
		InstructionNOP();
	};
}
