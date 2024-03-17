#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;

		template <size_t bit_width>
		class ALUSource
		{
		public:
			ALUSource();
			CoreUtils::InputPin<bit_width> reg_data;
			CoreUtils::InputPin<bit_width> fw_data;
			CoreUtils::InputPin<bit_width> opt_data;
			CoreUtils::InputPin<1> sel_reg;
			CoreUtils::InputPin<1> sel_forward;

			CoreUtils::OutputPin<bit_width> to_branch;
			CoreUtils::OutputPin<bit_width> to_alu;

		private:
			CoreUtils::Multiplexer<bit_width, 1> mux_branch;
			CoreUtils::Multiplexer<bit_width, 1> mux_alu;
		};

		template <size_t bit_width>
		ALUSource<bit_width>::ALUSource()
		{
			reg_data.on_state_change = [this](bitset<bit_width> new_data)
			{
				mux_branch.data_in[0].set_data(new_data);
			};
			fw_data.on_state_change = [this](bitset<bit_width> new_data)
			{
				mux_branch.data_in[1].set_data(new_data);
			};
			opt_data.on_state_change = [this](bitset<bit_width> new_data)
			{
				mux_alu.data_in[1].set_data(new_data);
			};
			sel_reg.on_state_change = [this](bitset<1> new_data)
			{
				mux_alu.select.set_data(new_data);
			};
			sel_forward.on_state_change = [this](bitset<1> new_data)
			{
				mux_branch.select.set_data(new_data);
			};
			mux_branch.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				to_branch.set_data(new_data);
				mux_alu.data_in[0].set_data(new_data);
			};
			mux_alu.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				to_alu.set_data(new_data);
			};
		}
	}
}