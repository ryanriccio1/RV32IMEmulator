#include "write_back.h"

#include "memory.h"

namespace RV32IM
{
	namespace Stage
	{
		WriteBack::WriteBack(Core* main_core): BaseStage(main_core) {}

		void WriteBack::clock()
		{
			reg_instruction = core->memory_stage->reg_instruction.read();
		}

		void WriteBack::run()
		{
			const Instruction instruction = core->memory_stage->reg_instruction;
			unsigned_data write_back_value;

			switch (instruction.opcode)
			{
			case RR:
			case RI:
			case LUI:
			case AUIPC:
				write_back_value = core->memory_stage->reg_alu;
				break;
			case JALR:
			case JAL:
				write_back_value = core->memory_stage->reg_PC + 4;
				break;
			case LX:
				write_back_value = core->memory_stage->reg_mem_in;
				break;
			default:
				write_back_value = 0xFFFFFFFF;
				break;
			}
			core->register_file.write(instruction.rd, write_back_value);
			reg_wb_value = write_back_value;
		}
	}
}
