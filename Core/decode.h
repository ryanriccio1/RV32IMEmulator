#pragma once
#include "base_stage.h"
#include "common.h"
#include "instruction.h"
#include "register.h"

namespace RV32IM
{
	namespace Stage
	{
		class Decode : virtual public BaseStage
		{
		public:
			friend class Fetch;
			friend class Execute;
			friend class Memory;
			friend class WriteBack;

			Decode(Core* core);
			void clock() override;
			void run() override;
			void insert_bubble(bool bubble);
			void stall(bool stall);
			void irq();

		private:
			Register<Instruction> reg_instruction;
			Register<unsigned_data> reg_rs1;
			Register<unsigned_data> reg_rs2;
			Register<unsigned_data> reg_PC;
			Register<unsigned_data> reg_predicted_PC;

			bool detect_hazard(unsigned_data reg, bool& forward, unsigned_data& forward_data) const;
			bool bubble;
			bool hazard;
			size_t irq_counter;
			unsigned_data irq_return_address;
		};
	}
}
