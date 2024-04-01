#include "fetch.h"

#include "core.h"

namespace RV32IM
{
	namespace Stage
	{
		Fetch::Fetch(Core* main_core) : BaseStage(main_core), PC(0), jump_occurred(false) {}

		void Fetch::clock()
		{
			reg_PC.clock();
			reg_predicted_PC.clock();
			reg_instruction.clock();
		}

		void Fetch::run()
		{
			unsigned_data temp_PC;
			// if jump occured, branch was predicted incorrectly
			// update everything with new address
			if (jump_occurred)
				temp_PC = PC;
			else  // otherwise use the predicted address
				temp_PC = reg_predicted_PC;

			// our prediction will be PC+4 unless we are loading a branch or jump instruction
			reg_PC = temp_PC;
			reg_predicted_PC = temp_PC + 4;

			const auto current_instruction = parse_instruction(core->memory->read_word(temp_PC));

			// if new instruction is a branch, ask BranchPredictor for a prediction
			if (current_instruction.opcode == Opcodes::BXX)
			{
				if (core->branch->take_branch(temp_PC))
					reg_predicted_PC = temp_PC + current_instruction.immediate;
			}

			// Unless interrupted by a future branch, unconditional jump = valid prediction
			if (current_instruction.type == InstructionFormat::J)
				reg_predicted_PC = temp_PC + current_instruction.immediate;

			// set the instruction data in pipeline register
			reg_instruction = current_instruction;
		}

		void Fetch::notify_jump(const bool jump, const unsigned_data pc)
		{
			// when jump occurs in future branch, we must go there
			PC = pc;
			jump_occurred = jump;
		}

		void Fetch::stall(const bool stall)
		{	// keep pipeline registers from being modified until unlocked
			reg_PC.set_write_enable(!stall);
			reg_predicted_PC.set_write_enable(!stall);
			reg_instruction.set_write_enable(!stall);
		}

		Instruction Fetch::parse_instruction(const inst_data& instruction_data)
		{
			switch (instruction_data & 0x7f)
			{
			case Opcodes::RR:
				return InstructionR(instruction_data);

			case Opcodes::RI:
			case Opcodes::LX:
			case Opcodes::JALR:
				return InstructionI(instruction_data);

			case Opcodes::SX:
				return InstructionS(instruction_data);

			case Opcodes::BXX:
				return InstructionB(instruction_data);

			case Opcodes::AUIPC:
			case Opcodes::LUI:
				return InstructionU(instruction_data);

			case Opcodes::JAL:
				return InstructionJ(instruction_data);

			default:
				return InstructionNOP();
			}
		}
	}
}