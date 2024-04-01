#include "execute.h"

#include "alu.h"
#include "decode.h"
#include "fetch.h"

namespace RV32IM
{
	namespace Stage
	{
		Execute::Execute(Core* main_core): BaseStage(main_core), invalid_prediction(false) {}

		void Execute::clock()
		{
			reg_instruction.clock();
			reg_alu.clock();
			reg_PC.clock();
			reg_rs2.clock();
		}

		void Execute::run()
		{
			// get data from previous stage
			const Instruction instruction = core->decode->reg_instruction;
			const unsigned_data rs1 = core->decode->reg_rs1;
			const unsigned_data rs2 = core->decode->reg_rs2;
			const unsigned_data pc = core->decode->reg_PC;

			const unsigned_data alu_result = ALU::get_result(instruction, rs1, rs2, pc);

			reg_instruction = instruction;
			reg_alu = alu_result;
			reg_PC = pc;
			reg_rs2 = rs2;

			unsigned_data next_pc;

			switch (instruction.opcode)
			{
			case Opcodes::BXX:
			case Opcodes::JAL:
			case Opcodes::JALR:
				next_pc = alu_result;
				if (core->decode->reg_predicted_PC != next_pc)
					invalid_prediction = true;
				break;
			default:
				next_pc = pc + 4;
				invalid_prediction = false;
			}
			// if branch instruction, a branch only occurs if the next PC != pc + 4
			if (instruction.opcode == Opcodes::BXX)
				core->branch.update_table(pc, next_pc != pc + 4); // keep record of branch for current address

			if (invalid_prediction)
			{
				core->decode->insert_bubble(true);
				core->fetch->notify_jump(true, next_pc);
			}
			else
			{
				core->decode->insert_bubble(false);
				core->fetch->notify_jump(false, next_pc);
			}
		}
	}
}
