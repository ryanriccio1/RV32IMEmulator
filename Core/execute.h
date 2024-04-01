#pragma once
#include "base_stage.h"
#include "common.h"
#include "instruction.h"
#include "register.h"

namespace RV32IM
{
	namespace Stage
	{
		class Execute : virtual public BaseStage
		{
		public:
			friend class Fetch;
			friend class Decode;
			friend class Memory;
			friend class WriteBack;

			Execute(Core* core);
			void clock() override;
			void run() override;

		private:
			Register<Instruction> reg_instruction;
			Register<unsigned_data> reg_alu;
			Register<unsigned_data> reg_PC;
			Register<unsigned_data> reg_rs2;

			bool invalid_prediction;
		};
	}
}
