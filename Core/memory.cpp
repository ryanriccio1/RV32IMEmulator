#include "memory.h"

#include "execute.h"
#include "core.h"

namespace RV32IM
{
	namespace Stage
	{
		Memory::Memory(Core* main_core): BaseStage(main_core) {}

		void Memory::clock()
		{
			reg_instruction.clock();
			reg_alu.clock();
			reg_mem_in.clock();
			reg_PC.clock();
		}

		void Memory::run()
		{
			const Instruction instruction = core->execute->reg_instruction;
			const unsigned_data alu_result = core->execute->reg_alu;
			const unsigned_data pc = core->execute->reg_PC;

			reg_instruction = instruction;
			reg_alu = alu_result;
			reg_PC = pc;

			if (instruction.opcode == Opcodes::SX)
			{
				const unsigned_data rs2 = core->execute->reg_rs2;
				switch (instruction.funct3)
				{
				case SW:
					core->memory->write_word(alu_result, rs2);
					break;
				case SH:
					core->memory->write_half_word(alu_result, static_cast<uint16_t>(rs2));
					break;
				case SB:
					core->memory->write_byte(alu_result, static_cast<uint8_t>(rs2));
					break;
				default:
					break;
				}
			}

			if (instruction.opcode == Opcodes::LX)
			{
				unsigned_data memory_in;
				switch (instruction.funct3)
				{
				case LB:
					memory_in = static_cast<int8_t>(core->memory->read_byte(alu_result));
					break;
				case LH:
					memory_in = static_cast<int16_t>(core->memory->read_half_word(alu_result));
					break;
				case LW:
					memory_in = core->memory->read_word(alu_result);
					break;
				case LBU:
					memory_in = core->memory->read_byte(alu_result);
					break;
				case LHU:
					memory_in = core->memory->read_half_word(alu_result);
					break;
				default:
					memory_in = 0xFFFFFFFF;
					break;
				}
				reg_mem_in = memory_in;
			}
		}
	}
}
