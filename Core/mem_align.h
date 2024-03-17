#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;

		template <size_t bit_width>
		class MemAlign
		{
		public:
			MemAlign();
			CoreUtils::InputPin<bit_width> data_in;
			CoreUtils::InputPin<3> funct3;

			CoreUtils::OutputPin<bit_width> data_out;

		private:
			CoreUtils::Multiplexer<32, 3> mux1;
		};

		template <size_t bit_width>
		MemAlign<bit_width>::MemAlign()
		{
			mux1.data_in[3].set_data(0);
			mux1.data_in[6].set_data(0);
			mux1.data_in[7].set_data(0);

			funct3.on_state_change = [this](bitset<3> new_data)
			{
				mux1.select.set_data(new_data);
			};
			mux1.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				data_out.set_data(new_data);
			};
			data_in.on_state_change = [this](bitset<bit_width> new_data)
			{
				mux1.data_in[0].set_data(CoreUtils::SignExtend<8, 32>::calc_sign_extend(static_cast<uint8_t>(new_data.to_ullong()))); // LB
				mux1.data_in[1].set_data(CoreUtils::SignExtend<16, 32>::calc_sign_extend(static_cast<uint16_t>(new_data.to_ullong()))); // LH
				mux1.data_in[2].set_data(new_data); // LW
				mux1.data_in[4].set_data(static_cast<uint8_t>(new_data.to_ullong())); // LBU
				mux1.data_in[5].set_data(static_cast<uint16_t>(new_data.to_ullong())); // LHU
			};
		}
	}
}