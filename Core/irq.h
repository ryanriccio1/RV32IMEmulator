#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;

		template <size_t bit_width>
		class IRQControl
		{
		public:
			IRQControl();
			CoreUtils::InputPin<1> irq_in;
			CoreUtils::InputPin<1> reset;
			CoreUtils::InputPin<1> clock;

			CoreUtils::InputPin<1> insert_nop;
			CoreUtils::InputPin<1> pc_stop;
			CoreUtils::InputPin<bit_width> instruction_out;


		private:
			CoreUtils::Counter<3, 4> nop_counter;
			CoreUtils::Counter<3, 5> pc_counter;
			CoreUtils::ORGate<1> reset_or;
			CoreUtils::ORGate<1> pc_stop_or;
			CoreUtils::ANDGate<1> nop_and;
			CoreUtils::ANDGate<1> pc_and;
			CoreUtils::ANDGate<1> buffer_and;
		};

		template <size_t bit_width>
		IRQControl<bit_width>::IRQControl()
		{
			clock.on_state_change = [this](bitset<1> new_data)
			{
				nop_counter.clock.set_data(new_data);
				pc_counter.clock.set_data(new_data);
			};
			reset.on_state_change = [this](bitset<1> new_data)
			{
				reset_or.b.set_data(new_data);
			};
			reset_or.data_out.on_state_change = [this](bitset<1> new_data)
			{
				nop_counter.reset.set_data(new_data);
				pc_counter.reset.set_data(new_data);
			};
			irq_in.on_state_change = [this](bitset<1> new_data)
			{
				reset_or.a.set_data(new_data.flip());
				nop_counter.write_enable.set_data(new_data);
				pc_counter.write_enable.set_data(new_data);
				nop_and.a.set_data(new_data);
				pc_and.a.set_data(new_data);
			};
			nop_counter.data_out.on_state_change = [this](bitset<1> new_data)
			{
				nop_and.b.set_data(new_data.flip());
			};
			pc_counter.data_out.on_state_change = [this](bitset<1> new_data)
			{
				pc_and.b.set_data(new_data.flip());
			};
			nop_and.data_out.on_state_change = [this](bitset<1> new_data)
			{
				insert_nop.set_data(new_data);
				pc_stop_or.a.set_data(new_data);
				buffer_and.a.set_data(new_data.flip());
			};
			pc_and.data_out.on_state_change = [this](bitset<1> new_data)
			{
				pc_stop_or.b.set_data(new_data);
				buffer_and.b.set_data(new_data.flip());
			};
			pc_stop_or.data_out.on_state_change = [this](bitset<1> new_data)
			{
				pc_stop.set_data(new_data);
			};
			buffer_and.data_out.on_state_change = [this](bitset<1> new_data)
			{
				if (new_data == 1)
					instruction_out.set_data(0x10000267);
			};
		}
	}
}