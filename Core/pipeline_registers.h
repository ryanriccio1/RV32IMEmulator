#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;

		template <size_t bit_width>
		class IF_ID
		{
		public:
			IF_ID();
			CoreUtils::InputPin<bit_width> PC;
			CoreUtils::InputPin<bit_width> PC4;
			CoreUtils::InputPin<bit_width> instruction;
			CoreUtils::InputPin<1> PC_en;
			CoreUtils::InputPin<1> clock;
			CoreUtils::InputPin<1> reset;

			CoreUtils::OutputPin<bit_width> PC_out;
			CoreUtils::OutputPin<bit_width> PC4_out;
			CoreUtils::OutputPin<bit_width> instruction_out;
			CoreUtils::OutputPin<5> rs1;
			CoreUtils::OutputPin<5> rs2;

		private:
			CoreUtils::Register<bit_width> PC_reg;
			CoreUtils::Register<bit_width> PC4_reg;
			CoreUtils::Register<bit_width> instruction_reg;
			CoreUtils::Multiplexer<bit_width, 1> mux1;
			CoreUtils::Splitter<bit_width, 15, 19> splitter_rs1;
			CoreUtils::Splitter<bit_width, 20, 24> splitter_rs2;

		};

		template <size_t bit_width>
		IF_ID<bit_width>::IF_ID()
		{
			PC_reg.write_enable.set_data(1);
			PC4_reg.write_enable.set_data(1);
			instruction_reg.write_enable.set_data(1);

			PC.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC_reg.data_in.set_data(new_data);
			};

			PC4.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC4_reg.data_in.set_data(new_data);
			};

			mux1.data_in[0].set_data(0x13);
			instruction.on_state_change = [this](bitset<bit_width> new_data)
			{
				mux1.data_in[1].set_data(new_data);
			};
			mux1.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				instruction_reg.data_in.set_data(new_data);
			};

			PC_en.on_state_change = [this](bitset<1> new_data)
			{
				mux1.select.set_data(new_data);
			};

			clock.on_state_change = [this](bitset<1> new_data)
			{
				PC_reg.clock.set_data(new_data);
				PC4_reg.clock.set_data(new_data);
				instruction_reg.clock.set_data(new_data);
			};

			reset.on_state_change = [this](bitset<1> new_data)
			{
				PC_reg.reset.set_data(new_data);
				PC4_reg.reset.set_data(new_data);
				instruction_reg.reset.set_data(new_data);
			};

			PC_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC_out.set_data(new_data);
			};
			PC4_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC4_out.set_data(new_data);
			};
			instruction_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				instruction_out.set_data(new_data);
				splitter_rs1.data_in.set_data(new_data);
				splitter_rs2.data_in.set_data(new_data);
			};
			splitter_rs1.data_out.on_state_change = [this](bitset<5> new_data)
			{
				rs1.set_data(new_data);
			};
			splitter_rs2.data_out.on_state_change = [this](bitset<5> new_data)
			{
				rs2.set_data(new_data);
			};
		}

		template <size_t bit_width>
		class ID_EX
		{
		public:
			ID_EX();
			CoreUtils::InputPin<bit_width> reg_a;
			CoreUtils::InputPin<bit_width> reg_b;
			CoreUtils::InputPin<bit_width> instruction;
			CoreUtils::InputPin<bit_width> PC;
			CoreUtils::InputPin<bit_width> PC4;
			CoreUtils::InputPin<bit_width> immediate;
			CoreUtils::InputPin<11> control_word;
			CoreUtils::InputPin<1> clock;
			CoreUtils::InputPin<1> reset;

			CoreUtils::OutputPin<bit_width> reg_a_out;
			CoreUtils::OutputPin<bit_width> reg_b_out;
			CoreUtils::OutputPin<5> reg_dest;
			CoreUtils::OutputPin<3> funct3;
			CoreUtils::OutputPin<5> rs1;
			CoreUtils::OutputPin<5> rs2;
			CoreUtils::OutputPin<7> funct7;
			CoreUtils::OutputPin<bit_width> PC_out;
			CoreUtils::OutputPin<bit_width> PC4_out;
			CoreUtils::OutputPin<bit_width> immediate_out;
			CoreUtils::OutputPin<11> control_word_out;
			CoreUtils::OutputPin<1> jump;
			CoreUtils::OutputPin<1> branch;
			CoreUtils::OutputPin<2> alu_op;
			CoreUtils::OutputPin<1> alu_src_1;
			CoreUtils::OutputPin<1> alu_src_2;

		private:
			CoreUtils::Register<bit_width> reg_a_reg;
			CoreUtils::Register<bit_width> reg_b_reg;
			CoreUtils::Register<bit_width> instruction_reg;
			CoreUtils::Register<bit_width> PC_reg;
			CoreUtils::Register<bit_width> PC4_reg;
			CoreUtils::Register<bit_width> immediate_reg;
			CoreUtils::Register<11> control_word_reg;

			CoreUtils::Splitter<bit_width, 7, 11> reg_dest_split;
			CoreUtils::Splitter<bit_width, 12, 14> funct3_split;
			CoreUtils::Splitter<bit_width, 15, 19> rs1_split;
			CoreUtils::Splitter<bit_width, 20, 24> rs2_split;
			CoreUtils::Splitter<bit_width, 25, 31> funct7_split;

			CoreUtils::Splitter<11, 0, 0> jump_split;
			CoreUtils::Splitter<11, 1, 1> branch_split;
			CoreUtils::Splitter<11, 5, 6> alu_op_split;
			CoreUtils::Splitter<11, 7, 7> alu_src_1_split;
			CoreUtils::Splitter<11, 8, 8> alu_src_2_split;

		};

		template <size_t bit_width>
		ID_EX<bit_width>::ID_EX()
		{
			reg_a_reg.write_enable.set_data(1);
			reg_b_reg.write_enable.set_data(1);
			instruction_reg.write_enable.set_data(1);
			PC_reg.write_enable.set_data(1);
			PC4_reg.write_enable.set_data(1);
			immediate_reg.write_enable.set_data(1);
			control_word_reg.write_enable.set_data(1);

			reg_a.on_state_change = [this](bitset<bit_width> new_data)
			{
				reg_a_reg.data_in.set_data(new_data);
			};
			reg_b.on_state_change = [this](bitset<bit_width> new_data)
			{
				reg_b_reg.data_in.set_data(new_data);
			};
			instruction.on_state_change = [this](bitset<bit_width> new_data)
			{
				instruction_reg.data_in.set_data(new_data);
			};
			PC.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC_reg.data_in.set_data(new_data);
			};
			PC4.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC4_reg.data_in.set_data(new_data);
			};
			immediate.on_state_change = [this](bitset<bit_width> new_data)
			{
				immediate_reg.data_in.set_data(new_data);
			};
			control_word.on_state_change = [this](bitset<11> new_data)
			{
				control_word_reg.data_in.set_data(new_data);
			};
			clock.on_state_change = [this](bitset<1> new_data)
			{
				reg_a_reg.clock.set_data(new_data);
				reg_b_reg.clock.set_data(new_data);
				instruction_reg.clock.set_data(new_data);
				PC_reg.clock.set_data(new_data);
				PC4_reg.clock.set_data(new_data);
				immediate_reg.clock.set_data(new_data);
				control_word_reg.clock.set_data(new_data);
			};
			reset.on_state_change = [this](bitset<1> new_data)
			{
				reg_a_reg.reset.set_data(new_data);
				reg_b_reg.reset.set_data(new_data);
				instruction_reg.reset.set_data(new_data);
				PC_reg.reset.set_data(new_data);
				PC4_reg.reset.set_data(new_data);
				immediate_reg.reset.set_data(new_data);
				control_word_reg.reset.set_data(new_data);
			};
			reg_a_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
					reg_a_out.set_data(new_data);
			};
			reg_b_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				reg_b_out.set_data(new_data);
			};
			instruction_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				reg_dest_split.data_in.set_data(new_data);
				funct3_split.data_in.set_data(new_data);
				rs1_split.data_in.set_data(new_data);
				rs2_split.data_in.set_data(new_data);
				funct7_split.data_in.set_data(new_data);
			};
			PC_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC_out.set_data(new_data);
			};
			PC4_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC4_out.set_data(new_data);
			};
			immediate_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				immediate_out.set_data(new_data);
			};
			control_word_reg.data_out.on_state_change = [this](bitset<11> new_data)
			{
				control_word_out.set_data(new_data);
				jump_split.data_in.set_data(new_data);
				branch_split.data_in.set_data(new_data);
				alu_op_split.data_in.set_data(new_data);
				alu_src_1_split.data_in.set_data(new_data);
				alu_src_2_split.data_in.set_data(new_data);
			};

			reg_dest_split.data_out.on_state_change = [this](bitset<5> new_data)
			{
				reg_dest.set_data(new_data);
			};
			funct3_split.data_out.on_state_change = [this](bitset<3> new_data)
			{
				funct3.set_data(new_data);
			};
			rs1_split.data_out.on_state_change = [this](bitset<5> new_data)
			{
				rs1.set_data(new_data);
			};
			rs2_split.data_out.on_state_change = [this](bitset<5> new_data)
			{
				rs2.set_data(new_data);
			};
			funct7_split.data_out.on_state_change = [this](bitset<7> new_data)
			{
				funct7.set_data(new_data);
			};

			jump_split.data_out.on_state_change = [this](bitset<1> new_data)
			{
				jump.set_data(new_data);
			};
			branch_split.data_out.on_state_change = [this](bitset<1> new_data)
			{
				branch.set_data(new_data);
			};
			alu_op_split.data_out.on_state_change = [this](bitset<2> new_data)
			{
				alu_op.set_data(new_data);
			};
			alu_src_1_split.data_out.on_state_change = [this](bitset<1> new_data)
			{
				alu_src_1.set_data(new_data);
			};
			alu_src_2_split.data_out.on_state_change = [this](bitset<1> new_data)
			{
				alu_src_2.set_data(new_data);
			};
		}

		template <size_t bit_width>
		class EX_MEM
		{
		public:
			EX_MEM();
			CoreUtils::InputPin<bit_width> alu_result;
			CoreUtils::InputPin<3> funct3;
			CoreUtils::InputPin<bit_width> reg_b;
			CoreUtils::InputPin<5> reg_dest;
			CoreUtils::InputPin<bit_width> PC4;
			CoreUtils::InputPin<11> control_word;
			CoreUtils::InputPin<1> jump_branch;
			CoreUtils::InputPin<1> clock;
			CoreUtils::InputPin<1> reset;

			CoreUtils::OutputPin<bit_width> alu_result_out;
			CoreUtils::OutputPin<3> funct3_out;
			CoreUtils::OutputPin<bit_width> reg_b_out;
			CoreUtils::OutputPin<5> reg_dest_out;
			CoreUtils::OutputPin<bit_width> PC4_out;
			CoreUtils::OutputPin<11> control_word_out;
			CoreUtils::OutputPin<1> mem_read;
			CoreUtils::OutputPin<1> mem_write;
			CoreUtils::OutputPin<1> rf_we;
			CoreUtils::OutputPin<1> jump_branch_out;

		private:
			CoreUtils::Register<bit_width> alu_result_reg;
			CoreUtils::Register<3> funct3_reg;
			CoreUtils::Register<bit_width> reg_b_reg;
			CoreUtils::Register<5> reg_dest_reg;
			CoreUtils::Register<bit_width> PC4_reg;
			CoreUtils::Register<11> control_word_reg;
			CoreUtils::Register<1> jump_branch_reg;

			CoreUtils::Splitter<11, 9, 9> mem_read_split;
			CoreUtils::Splitter<11, 10, 10> mem_write_split;
			CoreUtils::Splitter<11, 4, 4> rf_we_split;
		};

		template <size_t bit_width>
		EX_MEM<bit_width>::EX_MEM()
		{
			alu_result_reg.write_enable.set_data(1);
			funct3_reg.write_enable.set_data(1);
			reg_b_reg.write_enable.set_data(1);
			reg_dest_reg.write_enable.set_data(1);
			PC4_reg.write_enable.set_data(1);
			control_word_reg.write_enable.set_data(1);
			jump_branch_reg.write_enable.set_data(1);

			alu_result.on_state_change = [this](bitset<bit_width> new_data)
			{
				alu_result_reg.data_in.set_data(new_data);
			};
			funct3.on_state_change = [this](bitset<3> new_data)
			{
				funct3_reg.data_in.set_data(new_data);
			};
			reg_b.on_state_change = [this](bitset<bit_width> new_data)
			{
				reg_b_reg.data_in.set_data(new_data);
			};
			reg_dest.on_state_change = [this](bitset<5> new_data)
			{
				reg_dest_reg.data_in.set_data(new_data);
			};
			PC4.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC4_reg.data_in.set_data(new_data);
			};
			control_word.on_state_change = [this](bitset<11> new_data)
			{
				control_word_reg.data_in.set_data(new_data);
			};
			jump_branch.on_state_change = [this](bitset<1> new_data)
			{
				jump_branch_reg.data_in.set_data(new_data);
			};
			clock.on_state_change = [this](bitset<1> new_data)
			{
				funct3_reg.clock.set_data(new_data);
				reg_b_reg.clock.set_data(new_data);
				reg_dest_reg.clock.set_data(new_data);
				PC4_reg.clock.set_data(new_data);
				control_word_reg.clock.set_data(new_data);
				jump_branch_reg.clock.set_data(new_data);
				alu_result_reg.clock.set_data(new_data);
			};
			reset.on_state_change = [this](bitset<1> new_data)
			{
				alu_result_reg.reset.set_data(new_data);
				funct3_reg.reset.set_data(new_data);
				reg_b_reg.reset.set_data(new_data);
				reg_dest_reg.reset.set_data(new_data);
				PC4_reg.reset.set_data(new_data);
				control_word_reg.reset.set_data(new_data);
				jump_branch_reg.reset.set_data(new_data);
			};

			alu_result_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
					alu_result_out.set_data(new_data);
			};
			funct3_reg.data_out.on_state_change = [this](bitset<3> new_data)
			{
				funct3_out.set_data(new_data);
			};
			reg_b_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				reg_b_out.set_data(new_data);
			};
			reg_dest_reg.data_out.on_state_change = [this](bitset<5> new_data)
			{
				reg_dest_out.set_data(new_data);
			};
			PC4_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC4_out.set_data(new_data);
			};
			control_word_reg.data_out.on_state_change = [this](bitset<11> new_data)
			{
				control_word_out.set_data(new_data);
				mem_read_split.data_in.set_data(new_data);
				mem_write_split.data_in.set_data(new_data);
				rf_we_split.data_in.set_data(new_data);
			};
			jump_branch_reg.data_out.on_state_change = [this](bitset<1> new_data)
			{
				jump_branch_out.set_data(new_data);
			};

			mem_read_split.data_out.on_state_change = [this](bitset<1> new_data)
			{
				mem_read.set_data(new_data);
			};
			mem_write_split.data_out.on_state_change = [this](bitset<1> new_data)
			{
				mem_write.set_data(new_data);
			};
			rf_we_split.data_out.on_state_change = [this](bitset<1> new_data)
			{
				rf_we.set_data(new_data);
			};
		}

		template <size_t bit_width>
		class MEM_WB
		{
		public:
			MEM_WB();
			CoreUtils::InputPin<bit_width> alu_result;
			CoreUtils::InputPin<bit_width> PC4;
			CoreUtils::InputPin<bit_width> memory;
			CoreUtils::InputPin<5> reg_dest;
			CoreUtils::InputPin<11> control_word;
			CoreUtils::InputPin<1> clock;
			CoreUtils::InputPin<1> reset;

			CoreUtils::OutputPin<bit_width> alu_result_out;
			CoreUtils::OutputPin<bit_width> PC4_out;
			CoreUtils::OutputPin<bit_width> memory_out;
			CoreUtils::OutputPin<5> reg_dest_out;
			CoreUtils::OutputPin<2 > rf_src;
			CoreUtils::OutputPin<1> rf_we;

		private:
			CoreUtils::Register<bit_width> alu_result_reg;
			CoreUtils::Register<bit_width> PC4_reg;
			CoreUtils::Register<bit_width> memory_reg;
			CoreUtils::Register<5> reg_dest_reg;
			CoreUtils::Register<11> control_word_reg;

			CoreUtils::Splitter<11, 2, 3> rf_src_split;
			CoreUtils::Splitter<11, 4, 4> rf_we_split;
		};

		template <size_t bit_width>
		MEM_WB<bit_width>::MEM_WB()
		{
			alu_result_reg.write_enable.set_data(1);
			PC4_reg.write_enable.set_data(1);
			memory_reg.write_enable.set_data(1);
			reg_dest_reg.write_enable.set_data(1);
			control_word_reg.write_enable.set_data(1);

			alu_result.on_state_change = [this](bitset<bit_width> new_data)
			{
				alu_result_reg.data_in.set_data(new_data);
			};
			PC4.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC4_reg.data_in.set_data(new_data);
			};
			memory.on_state_change = [this](bitset<bit_width> new_data)
			{
				memory_reg.data_in.set_data(new_data);
			};
			reg_dest.on_state_change = [this](bitset<5> new_data)
			{
				reg_dest_reg.data_in.set_data(new_data);
			};
			control_word.on_state_change = [this](bitset<11> new_data)
			{
				control_word_reg.data_in.set_data(new_data);
			};
			clock.on_state_change = [this](bitset<1> new_data)
			{
				alu_result_reg.clock.set_data(new_data);
				PC4_reg.clock.set_data(new_data);
				memory_reg.clock.set_data(new_data);
				reg_dest_reg.clock.set_data(new_data);
				control_word_reg.clock.set_data(new_data);
			};
			reset.on_state_change = [this](bitset<1> new_data)
			{
				alu_result_reg.reset.set_data(new_data);
				PC4_reg.reset.set_data(new_data);
				memory_reg.reset.set_data(new_data);
				reg_dest_reg.reset.set_data(new_data);
				control_word_reg.reset.set_data(new_data);
			};

			alu_result_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				alu_result_out.set_data(new_data);
			};
			PC4_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				PC4_out.set_data(new_data);
			};
			memory_reg.data_out.on_state_change = [this](bitset<bit_width> new_data)
			{
				memory_out.set_data(new_data);
			};
			reg_dest_reg.data_out.on_state_change = [this](bitset<5> new_data)
			{
				reg_dest_out.set_data(new_data);
			};
			control_word_reg.data_out.on_state_change = [this](bitset<11> new_data)
			{
				rf_src_split.data_in.set_data(new_data);
				rf_we_split.data_in.set_data(new_data);
			};
			rf_src_split.data_out.on_state_change = [this](bitset<2> new_data)
			{
				rf_src.set_data(new_data);
			};
			rf_we_split.data_out.on_state_change = [this](bitset<1> new_data)
			{
				rf_we.set_data(new_data);
			};

		}
	}
}
