#pragma once
#include "base_stage.h"
#include "common.h"
#include "instruction.h"
#include "register.h"

namespace RV32IM
{
	namespace Stage
	{
		class WriteBack : virtual public BaseStage
		{
		public:
			friend class Fetch;
			friend class Decode;
			friend class Execute;
			friend class Memory;

			WriteBack(Core* core);
			void clock() override;
			void run() override;

		private:
			Register<Instruction> reg_instruction;
			Register<unsigned_data> reg_wb_value;
		};
	}
}
