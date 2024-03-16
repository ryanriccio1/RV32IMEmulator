#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;
		template <size_t bit_width>
		class ProgramCounter
		{
		public:
			ProgramCounter();
			CoreUtils::InputPin<1> PC_src;
			CoreUtils::InputPin<bit_width> PC_in;
			CoreUtils::InputPin<1> PC_en;
			CoreUtils::InputPin<1> clock;
			CoreUtils::InputPin<1> reset;
			CoreUtils::InputPin<1> irq;
			CoreUtils::OutputPin<bit_width> PC;
			CoreUtils::OutputPin<bit_width> PC4;
		private:
			CoreUtils::Multiplexer<bit_width, 1> mux1;
			CoreUtils::Multiplexer<bit_width, 1> mux2;
			CoreUtils::Multiplexer<bit_width, 1> mux3;
			CoreUtils::Register<bit_width> reg;
		};

		template <size_t bit_width>
		ProgramCounter<bit_width>::ProgramCounter()
		{
			PC_src.on_state_change = [this](bitset<1> new_data)
			{
				this->mux1.select.set_data(new_data);
			};
			PC_in.on_state_change = [this](bitset<bit_width> new_data)
			{
				new_data &= ~1;
				this->mux1.data_in[1].set_data(new_data);
			};
			PC_en.on_state_change = [this](bitset<1> new_data)
			{
				this->reg.write_enable.set_data(new_data);
			};
			clock.on_state_change = [this](bitset<1> new_data)
			{
				this->reg.clock.set_data(new_data);
			};
			reset.on_state_change = [this](bitset<1> new_data)
			{
				this->reg.reset.set_data(new_data);
			};
			irq.on_state_change = [this](bitset<1> new_data)
			{
				this->mux2.select.set_data(new_data);
				this->mux3.select.set_data(new_data);
			};
			mux1.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				this->reg.data_in.set_data(new_data);
			};
			mux2.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				this->PC4.set_data(new_data);
			};
			mux3.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				this->mux1.data_in[0].set_data(new_data);
			};
			reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				this->PC.set_data(new_data);
				this->mux2.data_in[1].set_data(new_data);
				this->mux3.data_in[1].set_data(new_data);
				bitset<bit_width> PC4 = new_data.to_ullong() + 4;
				this->mux2.data_in[0].set_data(PC4);
				this->mux3.data_in[0].set_data(PC4);
			};
			PC4.set_data(4);
		}
	}
}
