#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;

		template <size_t bit_width>
		class ForwardControl
		{
		public:
			ForwardControl();
			CoreUtils::InputPin<5> wb_reg_dest;
			CoreUtils::InputPin<5> rs1;
			CoreUtils::InputPin<5> mem_reg_dest;
			CoreUtils::InputPin<5> rs2;
			CoreUtils::InputPin<1> mem_reg_write;
			CoreUtils::InputPin<1> wb_reg_write;
			CoreUtils::InputPin<bit_width> data_wb;
			CoreUtils::InputPin<bit_width> data_mem;

			CoreUtils::OutputPin<1> fw_en1;
			CoreUtils::OutputPin<1> fw_en2;
			CoreUtils::OutputPin<bit_width> fw_data1;
			CoreUtils::OutputPin<bit_width> fw_data2;

		private:
			void calculate_fw();
			CoreUtils::ORGate<1> fw1_or_gate;
			CoreUtils::ORGate<1> fw2_or_gate;
			CoreUtils::Multiplexer<bit_width, 1> fw_data1_mux;
			CoreUtils::Multiplexer<bit_width, 1> fw_data2_mux;
		};

		template <size_t bit_width>
		ForwardControl<bit_width>::ForwardControl()
		{
			wb_reg_dest.on_state_change = [this](bitset<5> new_data)
				{
					calculate_fw();
				};
			rs1.on_state_change = [this](bitset<5> new_data)
				{
					calculate_fw();
				};
			mem_reg_dest.on_state_change = [this](bitset<5> new_data)
				{
					calculate_fw();
				};
			rs2.on_state_change = [this](bitset<5> new_data)
				{
					calculate_fw();
				};
			fw_data1_mux.data_out.on_state_change = [this](bitset<bit_width> new_data)
				{
					fw_data1.set_data(new_data);
				};
			fw_data2_mux.data_out.on_state_change = [this](bitset<bit_width> new_data)
				{
					fw_data2.set_data(new_data);
				};
			data_wb.on_state_change = [this](bitset<bit_width> new_data)
				{
					fw_data1_mux.data_in[0].set_data(new_data);
					fw_data2_mux.data_in[0].set_data(new_data);
				};
			data_mem.on_state_change = [this](bitset<bit_width> new_data)
				{
					fw_data1_mux.data_in[1].set_data(new_data);
					fw_data2_mux.data_in[1].set_data(new_data);
				};
		}

		template <size_t bit_width>
		void ForwardControl<bit_width>::calculate_fw()
		{
			if (wb_reg_dest.get_data() == rs1.get_data() && 
				rs1.get_data().to_ullong() != 0 && 
				wb_reg_write.get_data().to_ullong() == 1)
			{
				fw1_or_gate.a.set_data(1);
			}
			else
			{
				fw1_or_gate.a.set_data(0);
			}

			if (mem_reg_dest.get_data() == rs1.get_data() &&
				mem_reg_dest.get_data().to_ullong() != 0 &&
				mem_reg_write.get_data().to_ullong() == 1)
			{
				fw1_or_gate.b.set_data(1);
				fw_data1_mux.select.set_data(1);
			}
			else
			{
				fw1_or_gate.b.set_data(0);
				fw_data1_mux.select.set_data(0);
			}

			if (wb_reg_dest.get_data() == rs2.get_data() &&
				wb_reg_dest.get_data().to_ullong() != 0 &&
				wb_reg_write.get_data().to_ullong() == 1)
			{
				fw2_or_gate.a.set_data(1);
			}
			else
			{
				fw2_or_gate.a.set_data(0);
			}

			if (mem_reg_dest.get_data() == rs2.get_data() &&
				mem_reg_dest.get_data().to_ullong() != 0 &&
				mem_reg_write.get_data().to_ullong() == 1)
			{
				fw2_or_gate.b.set_data(1);
				fw_data2_mux.select.set_data(1);
			}
			else
			{
				fw2_or_gate.b.set_data(0);
				fw_data2_mux.select.set_data(0);
			}
		}
	}
}