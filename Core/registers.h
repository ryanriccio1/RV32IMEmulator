#pragma once
#include <array>

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
			bitset<bit_width> registers[1 << sel_bits];
			CoreUtils::InputPin<bit_width> data_in;
			CoreUtils::InputPin<sel_bits> rs1;
			CoreUtils::InputPin<sel_bits> rs2;
			CoreUtils::InputPin<sel_bits> rd;
			CoreUtils::InputPin<1> we;
			CoreUtils::InputPin<1> clock;
			CoreUtils::InputPin<1> reset;

			CoreUtils::OutputPin<bit_width> a;
			CoreUtils::OutputPin<bit_width> b;
		};

		template <size_t bit_width, size_t sel_bits>
		Registers<bit_width, sel_bits>::Registers()
		{
			clock.on_state_change = [this](bitset<1> new_data)
			{
				if (new_data == 0 && we.get_data() == 1)
				{
					auto rd_data = rd.get_data().to_ullong();
					auto data_in_data = data_in.get_data();
					if (rd_data != 0)
					{
						registers[rd_data] = data_in_data;
						if (rd_data == rs1.get_data())
							a.set_data(data_in_data);
						if (rd_data == rs2.get_data())
							b.set_data(data_in_data);
					}
				}
			};
			reset.on_state_change = [this](bitset<1> new_data)
			{
				for (size_t idx{ 0 }; idx < (1 << sel_bits); idx++)
				{
					registers[idx] = 0;
				}
			};

			rs1.on_state_change = [this](bitset<sel_bits> new_data)
			{
				a.set_data(registers[new_data.to_ulong()]);
			};
			rs2.on_state_change = [this](bitset<sel_bits> new_data)
			{
				b.set_data(registers[new_data.to_ulong()]);
			};
		}
	}
}