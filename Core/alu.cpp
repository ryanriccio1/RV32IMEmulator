#include "alu.h"

#include <iostream>

namespace RV32IM
{
	unsigned_data ALU::get_result(const Instruction& instruction, const unsigned_data rs1, const unsigned_data rs2,
		const unsigned_data pc)
	{
		switch (instruction.type)
		{
		case InstructionFormat::R:
			if (instruction.opcode == Opcodes::RR)
				return get_calculation(instruction, rs1, rs2);
			break;

		case InstructionFormat::I:
			if (instruction.opcode == Opcodes::LX)
				return rs1 + instruction.immediate;
			if (instruction.opcode == Opcodes::JALR)
				return (rs1 + instruction.immediate) & 0xFFFFFFFE;
			if (instruction.opcode == Opcodes::RI)
				return get_calculation(instruction, rs1, rs2);
			break;

		case InstructionFormat::S:
			if (instruction.opcode == Opcodes::SX)
				return rs1 + instruction.immediate;
			break;

		case InstructionFormat::B:
			if (instruction.opcode == Opcodes::BXX)
				return get_calculation(instruction, rs1, rs2, pc);
			break;

		case InstructionFormat::U:
			if (instruction.opcode == Opcodes::LUI)
				return instruction.immediate;
			if (instruction.opcode == Opcodes::AUIPC)
				return instruction.immediate + pc;
			break;

		case InstructionFormat::J:
			if (instruction.opcode == Opcodes::JAL)
				return instruction.immediate + pc;
			break;
		}
		return 0xFFFFFFFF;
	}

	unsigned_data ALU::get_calculation(const Instruction& instruction, const unsigned_data rs1, unsigned_data rs2, const unsigned_data pc)
	{
		if (instruction.type == InstructionFormat::B)
		{
			const unsigned_data branch = pc + instruction.immediate;
			const unsigned_data no_branch = pc + 4;
			switch (instruction.funct3)
			{
			case BEQ:
				return rs1 == rs2 ? branch : no_branch;
			case BNE:
				return rs1 != rs2 ? branch : no_branch;
			case BLT:
				return static_cast<signed_data>(rs1) < static_cast<signed_data>(rs2) ? branch : no_branch;
			case BGE:
				return static_cast<signed_data>(rs1) >= static_cast<signed_data>(rs2) ? branch : no_branch;
			case BLTU:
				return rs1 < rs2 ? branch : no_branch;
			case BGEU:
				return rs1 >= rs2 ? branch : no_branch;
			}
		}
		if (instruction.type == InstructionFormat::I)
		{
			rs2 = instruction.immediate;
			if (instruction.immediate & (1 << 10) && instruction.funct3 == SRA)
			{
				return static_cast<signed_data>(rs1) >> (rs2 & 0x1F);
			}
		}

		switch (instruction.funct7)
		{
		case NORM:
			switch (instruction.funct3)
			{
			case ADD:
				return rs1 + rs2;
			case SLL:
				return rs1 << (rs2 & 0x1F);
			case SLT:
				return (static_cast<signed_data>(rs1) < static_cast<signed_data>(rs2)) ? 1 : 0;
			case SLTU:
				return (rs1 < rs2) ? 1 : 0;
			case XOR:
				return rs1 ^ rs2;
			case SRL:
				return rs1 >> (rs2 & 0x1F);
			case OR:
				return rs1 | rs2;
			case AND:
				return rs1 & rs2;
			}
		case INV:
			switch (instruction.funct3)
			{
			case SUB:
				return rs1 - rs2;
			case SRA:
				return static_cast<signed_data>(rs1) >> (rs2 & 0x1F);

			case SLL:
			case SLT:
			case SLTU:
			case XOR:
			case OR:
			case AND:
				assert(0 && "Invalid Operation");
				return 0xFFFFFFFF;
			}

		case M_EXT:
			switch (instruction.funct3)
			{
			case MUL:
				return rs1 * rs2;
			case MULH:
				return (static_cast<int64_t>(static_cast<signed_data>(rs1)) * static_cast<int64_t>(static_cast<signed_data>(rs2))) >> 32;
			case MULHSU:
				return (static_cast<int64_t>(static_cast<signed_data>(rs1)) * static_cast<int64_t>(rs2)) >> 32;
			case MULHU:
				return (static_cast<int64_t>(rs1) * static_cast<int64_t>(rs2)) >> 32;
			case DIV:
				if (rs2 == 0)
					return -1;
				if (static_cast<signed_data>(rs2) == -1 && static_cast<signed_data>(rs1) == INT32_MIN)
					return INT32_MIN;
				return static_cast<signed_data>(rs1) / static_cast<signed_data>(rs2);
			case REM:
				if (rs2 == 0)
					return rs1;
				if (static_cast<signed_data>(rs2) == -1 && static_cast<signed_data>(rs1) == INT32_MIN)
					return 0;
				return static_cast<signed_data>(rs1) % static_cast<signed_data>(rs2);
			case DIVU:
				if (rs2 == 0)
					return -1;
				return rs1 / rs2;
			case REMU:
				if (rs2 == 0)
					return rs1;
				return rs1 % rs2;
			}
		}
		return 0xFFFFFFFF;
	}
}
