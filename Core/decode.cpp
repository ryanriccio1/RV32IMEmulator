#include "decode.h"

#include "execute.h"
#include "fetch.h"
#include "memory.h"
#include "write_back.h"

namespace RV32IM
{
	namespace Stage
	{
		Decode::Decode(Core* main_core): BaseStage(main_core), bubble(false), hazard(false)	{}

		void Decode::clock()
		{
			reg_instruction.clock();
			reg_rs1.clock();
			reg_rs2.clock();
			reg_PC.clock();
			reg_predicted_PC.clock();
		}

		void Decode::run()
		{
			// if bubble is requested from other stage
			// or from this stage on a previous clock
			// insert NOP
			if (bubble)
			{
				reg_instruction = InstructionNOP();
				return;
			}

			// get data from previous stage
			reg_instruction = core->fetch->reg_instruction.read();
			reg_PC = core->fetch->reg_PC.read();
			reg_predicted_PC = core->fetch->reg_predicted_PC.read();
			hazard = false;

			const Instruction instruction = core->fetch->reg_instruction;

			bool forward;
			unsigned_data forward_data;
			unsigned_data reg_value = 0;

			if (instruction.has_rs1())
			{
				if (detect_hazard(instruction.rs1, forward, forward_data))
				{
					if (forward)
						reg_value = forward_data;
					else
						hazard = true;
				}
				else
				{
					reg_value = core->register_file.read(instruction.rs1);
				}
				reg_rs1 = reg_value;
			}

			if (instruction.has_rs2())
			{
				if (detect_hazard(instruction.rs2, forward, forward_data))
				{
					if (forward)
						reg_value = forward_data;
					else
						hazard = true;
				}
				else
				{
					reg_value = core->register_file.read(instruction.rs2);
				}
				reg_rs2 = reg_value;
			}

			core->fetch->stall(hazard);
			insert_bubble(hazard);
		}

		void Decode::insert_bubble(bool insert_bubble)
		{	// it is the responsibility of other stages to clear the bubble
			bubble = insert_bubble;
			if (bubble)
				reg_instruction = InstructionNOP();
		}

		void Decode::stall(const bool stall)
		{
			reg_instruction.set_write_enable(!stall);
			reg_rs1.set_write_enable(!stall);
			reg_rs2.set_write_enable(!stall);
			reg_PC.set_write_enable(!stall);
			reg_predicted_PC.set_write_enable(!stall);
		}

		bool Decode::detect_hazard(const unsigned_data reg, bool& forward, unsigned_data& forward_data) const
		{
			if (reg == 0)
				return false;
			const Instruction execute_instruction = core->execute->reg_instruction.get_input();
			const Instruction memory_instruction = core->memory_stage->reg_instruction.get_input();
			const Instruction write_back_instruction = core->write_back->reg_instruction.get_input();
			forward = false;

			if (execute_instruction.rd == reg)
			{
				if (execute_instruction.opcode == Opcodes::RI || 
					execute_instruction.opcode == Opcodes::RR)
				{
					forward = true;
					forward_data = core->execute->reg_alu.get_input();
				}
				return true;
			}
			if (memory_instruction.rd == reg)
			{
				if (memory_instruction.opcode == Opcodes::LX)
				{
					forward = true;
					forward_data = core->memory_stage->reg_mem_in.get_input();
				}
				if (memory_instruction.opcode == Opcodes::RI ||
					memory_instruction.opcode == Opcodes::RR)
				{
					forward = true;
					forward_data = core->memory_stage->reg_alu.get_input();
				}
				return true;
			}
			if (write_back_instruction.rd == reg)
			{
				forward = true;
				forward_data = core->write_back->reg_wb_value.get_input();
				return true;
			}
			return false;

		}
	}
}
