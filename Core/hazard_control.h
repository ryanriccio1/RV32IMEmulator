#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;

		class HazardControl
		{
		public:
			HazardControl();
			CoreUtils::InputPin<1> clock;
			CoreUtils::InputPin<1> reset;
			CoreUtils::InputPin<1> jump_branch_ex;
			CoreUtils::InputPin<1> jump_branch_mem;
			CoreUtils::InputPin<1> mem_read;
			CoreUtils::InputPin<1> mem_write;

			CoreUtils::OutputPin<1> flush;
			CoreUtils::OutputPin<1> PC_en;
			CoreUtils::OutputPin<1> address_source;

		private:
			CoreUtils::Register<1> reg1;
			CoreUtils::ORGate<1> or_reg_reset;
			CoreUtils::ORGate<1> or_mem_operation;
			CoreUtils::ORGate<1> or_pc_en;
		};

		inline HazardControl::HazardControl()
		{
			clock.on_state_change = [this](bitset<1> new_data)
			{
				reg1.clock.set_data(new_data);
			};
			reset.on_state_change = [this](bitset<1> new_data)
			{
				or_reg_reset.a.set_data(new_data);
			};			
			jump_branch_ex.on_state_change = [this](bitset<1> new_data)
			{
				reg1.data_in.set_data(new_data);
				or_pc_en.a.set_data(new_data);
			};
			jump_branch_mem.on_state_change = [this](bitset<1> new_data)
			{
				reg1.reset.set_data(new_data);
			};
			mem_read.on_state_change = [this](bitset<1> new_data)
			{
				or_mem_operation.a.set_data(new_data);
			};
			mem_write.on_state_change = [this](bitset<1> new_data)
			{
				or_mem_operation.b.set_data(new_data);
			};
			reg1.data_out.on_state_change = [this](bitset<1> new_data)
			{
				or_reg_reset.b.set_data(new_data);
			};
			or_reg_reset.data_out.on_state_change = [this](bitset<1> new_data)
			{
				flush.set_data(new_data);
			};
			or_mem_operation.data_out.on_state_change = [this](bitset<1> new_data)
			{
				or_pc_en.b.set_data(new_data.flip());
				address_source.set_data(new_data);
			};
			or_pc_en.data_out.on_state_change = [this](bitset<1> new_data)
			{
				PC_en.set_data(new_data);
			};
		}
	}
}