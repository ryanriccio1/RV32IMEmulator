#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;

		template <size_t bit_width>
		class BranchLogic
		{
		public:
			BranchLogic();
			CoreUtils::InputPin<bit_width> a;
			CoreUtils::InputPin<bit_width> b;
			CoreUtils::InputPin<3> funct3;

			CoreUtils::OutputPin<1> branch;

		private:
			CoreUtils::Multiplexer<1, 3> mux1;
			void set_branch_bits();
		};

		template <size_t bit_width>
		BranchLogic<bit_width>::BranchLogic()
		{
			mux1.data_in[2].set_data(0);
			mux1.data_in[3].set_data(0);

			funct3.on_state_change = [this](bitset<3> new_data)
			{
				mux1.select.set_data(new_data);
			};
			a.on_state_change = [this](bitset<bit_width> new_data)
			{
				set_branch_bits();
			};
			b.on_state_change = [this](bitset<bit_width> new_data)
			{
				set_branch_bits();
			};
			mux1.data_out.on_state_change = [this](bitset<1> new_data)
			{
				branch.set_data(new_data);
			};
		}

		template <size_t bit_width>
		void BranchLogic<bit_width>::set_branch_bits()
		{
			mux1.data_in[0].set_data(a.get_data() == b.get_data());	// beq
			mux1.data_in[1].set_data(a.get_data() != b.get_data());	// bne
			mux1.data_in[4].set_data(CoreUtils::bitset_to_int(a) < CoreUtils::bitset_to_int(b));	// blt
			mux1.data_in[5].set_data(CoreUtils::bitset_to_int(a) >= CoreUtils::bitset_to_int(b));	// bge
			mux1.data_in[6].set_data(a.get_data().to_ullong() < b.get_data().to_ullong());	// bltu
			mux1.data_in[7].set_data(a.get_data().to_ullong() >= b.get_data().to_ullong());	// bgeu
		}
	}
}