#pragma once
#include <vector>

#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;
		template <size_t bit_width, size_t sel_bits>
		class Registers
		{
		public:
			Registers();
			vector<CoreUtils::Register<bit_width>> registers;
			CoreUtils::InputPin<bit_width> data_in;
			CoreUtils::InputPin<sel_bits> rs1;
			CoreUtils::InputPin<sel_bits> rs2;
			CoreUtils::InputPin<sel_bits> rd;
			CoreUtils::InputPin<1> we;
			CoreUtils::InputPin<1> clock;
			CoreUtils::InputPin<1> reset;

			CoreUtils::OutputPin<bit_width> a;
			CoreUtils::OutputPin<bit_width> b;

		private:
			CoreUtils::Multiplexer<32, 5> rs1_mux;
			CoreUtils::Multiplexer<32, 5> rs2_mux;
		};

		template <size_t bit_width, size_t sel_bits>
		Registers<bit_width, sel_bits>::Registers()
		{
			for (size_t idx{ 0 }; idx < (1 << sel_bits); idx++)
			{
				registers.push_back(CoreUtils::Register<bit_width>(false));
				registers[idx].data_out.on_state_change = [this, idx](bitset<bit_width> new_data)
				{
					rs1_mux.data_in[idx].set_data(new_data);
					rs2_mux.data_in[idx].set_data(new_data);
				};
				registers[idx].data_out.set_data(0);
			}
			clock.on_state_change = [this](bitset<1> new_data)
			{
				for (auto& reg : registers)
				{
					reg.clock.set_data(new_data);
				}
			};
			reset.on_state_change = [this](bitset<1> new_data)
			{
				for (auto& reg : registers)
				{
					reg.reset.set_data(new_data);
				}
			};
			data_in.on_state_change = [this](bitset<bit_width> new_data)
			{
				for (auto& reg : registers)
				{
					reg.data_in.set_data(new_data);
				}
			};
			we.on_state_change = [this](bitset<1> new_data)
			{
				for (auto& reg : registers)
				{
					reg.write_enable.set_data(0);
				}
				if (new_data.to_ullong() == 1 && rd.get_data().to_ullong() != 0)
				{
					registers[rd.get_data().to_ullong()].write_enable.set_data(1);
				}
			};
			rs1.on_state_change = [this](bitset<sel_bits> new_data)
			{
				rs1_mux.select.set_data(new_data);
			};
			rs2.on_state_change = [this](bitset<sel_bits> new_data)
			{
				rs2_mux.select.set_data(new_data);
			};
			rs1_mux.data_out.on_state_change = [this](bitset<32> new_data)
			{
				a.set_data(new_data);
			};
			rs2_mux.data_out.on_state_change = [this](bitset<32> new_data)
			{
				b.set_data(new_data);
			};
		}
	}
}