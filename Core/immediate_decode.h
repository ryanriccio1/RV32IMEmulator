#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;

		template <size_t bit_width>
		class ImmediateDecode
		{
		public:
			ImmediateDecode();
			CoreUtils::InputPin<bit_width> instruction;
			CoreUtils::OutputPin<bit_width> immediate;
		};

		template <size_t bit_width>
		ImmediateDecode<bit_width>::ImmediateDecode()
		{
			instruction.on_state_change = [this](bitset<bit_width> new_data)
			{
				uint64_t data_val = new_data.to_ullong();
				switch ((data_val & 0x7C) >> 2)
				{
				case 0x00:
				case 0x19:
				case 0x04:
					// I-type
					data_val = CoreUtils::SignExtend<12, 32>::calc_sign_extend(data_val >> 20).to_ullong();
					break;

				case 0x08:
					// S-type
					data_val = (CoreUtils::SignExtend<7, 27>::calc_sign_extend((data_val & 0xFE000000) >> 25).to_ullong() << 5) | (data_val & 0xF80) >> 7;
					break;

				case 0x18:
					// B-type
					data_val = CoreUtils::SignExtend<1, 20>::calc_sign_extend(data_val >> 31).to_ullong() << 12 | (data_val & 0x7E000000) >> 20 | (data_val & 0xF00) >> 7 | (data_val & 0x80) << 4;
					break;

				case 0x0D:
				case 0x05:
				case 0x0C:
					// U-type
					data_val = (data_val >> 12) << 12;
					break;

				case 0x1B:
					// J-type
					data_val = CoreUtils::SignExtend<1, 12>::calc_sign_extend(data_val >> 31).to_ullong() << 20 | (data_val & 0xFF000) | (data_val & 0x100000) >> 9 | (data_val & 0x7FE00000) >> 20;
					break;
				default:
					data_val = 0xFFFFFFFF;
				}				
				immediate.set_data(data_val);
			};
			
		}
	}
}