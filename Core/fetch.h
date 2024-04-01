#pragma once
#include "base_stage.h"
#include "common.h"
#include "instruction.h"
#include "register.h"

namespace RV32IM
{
	namespace Stage
	{
		class Fetch : virtual public BaseStage
		{
		public:
			friend class Core;
			friend class Decode;
			friend class Execute;
			friend class Memory;
			friend class WriteBack;

			Fetch(Core* core);
			void clock() override;
			void run() override;
			void notify_jump(bool jump, unsigned_data pc);
			void stall(bool stall);

		private:
			Register<unsigned_data> reg_PC;
			Register<unsigned_data> reg_predicted_PC;
			Register<Instruction> reg_instruction;
			unsigned_data PC;
			bool jump_occurred;

			static Instruction parse_instruction(const inst_data& instruction_data);

		};
	}
}
