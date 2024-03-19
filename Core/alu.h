#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;

		template <size_t bit_width>
		class ALU
		{
		public:
			ALU();
			CoreUtils::InputPin<bit_width> a;
			CoreUtils::InputPin<bit_width> b;
			CoreUtils::InputPin<2> alu_op;
			CoreUtils::InputPin<7> funct7;
			CoreUtils::InputPin<3> funct3;

			CoreUtils::OutputPin<bit_width> result;

		private:
			bitset<bit_width> get_alu_output();
			void calculate_control_signals();
			bool invert = false;
			bool M_ext_en = false;
			bool funct3_en = false;
			bool bypass = false;
		};

		template <size_t bit_width>
		ALU<bit_width>::ALU()
		{
			alu_op.on_state_change = [this](bitset<2> new_data)
			{
				calculate_control_signals();
			};
			funct7.on_state_change = [this](bitset<7> new_data)
			{
				calculate_control_signals();
			};
			funct3.on_state_change = [this](bitset<3> new_data)
			{
				calculate_control_signals();
			};
			a.on_state_change = [this](bitset<bit_width> new_data)
			{
				result.set_data(get_alu_output());
			};
			b.on_state_change = [this](bitset<bit_width> new_data)
			{
				result.set_data(get_alu_output());
			};
		}

		template <size_t bit_width>
		bitset<bit_width> ALU<bit_width>::get_alu_output()
		{
			const auto a_dat = a.get_data().to_ullong();
			const auto b_dat = b.get_data().to_ullong();
			const auto a_dat_signed = CoreUtils::bitset_to_int(a.get_data());
			const auto b_dat_signed = CoreUtils::bitset_to_int(b.get_data());

			if (bypass)
				return b_dat;

			if (!funct3_en)
 				return a_dat + b_dat;

			switch (funct3.get_data().to_ullong())
			{
			case 0:
				if (!M_ext_en)
				{
					if (!invert)
						return a_dat + b_dat;	// add
					return a_dat - b_dat;		// sub	
				}
				return a_dat_signed * b_dat_signed;	// mul

			case 1:
				if (!M_ext_en)
					return a_dat << b_dat;	// sll
				return (a_dat_signed * b_dat_signed) >> 32;	// mulh

			case 2:
				if (!M_ext_en)
					return a_dat_signed < b_dat_signed;	// slt
				return (a_dat_signed * b_dat) >> 32;	// mulhsu

			case 3:
				if (!M_ext_en)
					return a_dat < b_dat;	// sltu
				return (a_dat * b_dat) >> 32;	// mulhu

			case 4:
				if (!M_ext_en)
					return a_dat ^ b_dat;	// xor
				return a_dat_signed / b_dat_signed;	// div

			case 5:
				if (!M_ext_en)
				{
					if (!invert)
						return a_dat >> b_dat;	// srl
					return a_dat_signed >> b_dat_signed;	// sra
				}
				return a_dat_signed % b_dat_signed;	// rem
	
			case 6:
				if (!M_ext_en)
					return a_dat | b_dat;	// or
				return a_dat / b_dat;	// divu

			case 7:
				if (!M_ext_en)
					return a_dat & b_dat;	// and
				return a_dat / b_dat;	// remu

			default:	// should never be hit, same as bypass
				return b_dat;
			}
		}

		template <size_t bit_width>
		void ALU<bit_width>::calculate_control_signals()
		{
			// default (add)
			if (alu_op.get_data() == 0b00)
			{
				invert = false;
				M_ext_en = false;
				funct3_en = false;
				bypass = false;
				return;
			}

			// I-type
			if (alu_op.get_data() == 0b10)
			{
				invert = false;
				M_ext_en = false;
				funct3_en = true;
				bypass = false;
				return;
			}

			// bypass
			if (alu_op.get_data() == 0b11)
			{
				invert = false;
				M_ext_en = false;
				funct3_en = true;
				bypass = true;
				return;
			}

			// R-type
			if (alu_op.get_data() == 0b01)
			{
				// r-type
				if (funct7.get_data() == 0x00)
				{
					invert = false;
					M_ext_en = false;
					funct3_en = true;
					bypass = false;
					return;
				}
				// inv-fx (r-type alt)
				if (funct7.get_data() == 0x20)
				{
					invert = true;
					M_ext_en = false;
					funct3_en = true;
					bypass = false;
					return;
				}
				// M-ext
				if (funct7.get_data() == 0x01)
				{
					invert = false;
					M_ext_en = true;
					funct3_en = true;
					bypass = false;
					return;
				}
			}			
		}
	}
}