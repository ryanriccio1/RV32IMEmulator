#pragma once
#include "../Core/utils.h"

namespace RISCV
{
	using namespace std;

	template <size_t bit_width>
	class MemoryManagementUnit
	{
	public:
		MemoryManagementUnit();
		CoreUtils::InputPin<bit_width> address;

		CoreUtils::OutputPin<1> memory_enable;
		CoreUtils::OutputPin<1> vga_enable;
		CoreUtils::OutputPin<1> pia_enable;
	};

	template <size_t bit_width>
	MemoryManagementUnit<bit_width>::MemoryManagementUnit()
	{
		address.on_state_change = [this](bitset<32> new_data)
		{
			memory_enable.set_data(0);
			vga_enable.set_data(0);
			pia_enable.set_data(0);

			if (new_data.to_ullong() >= 0x00000 || new_data.to_ullong() <= 0xEFFFF)
				memory_enable.set_data(1);
			if (new_data.to_ullong() >= 0xFFF00 || new_data.to_ullong() <= 0xFFF20)
				vga_enable.set_data(1);
			if (new_data.to_ullong() >= 0xFFFF0 || new_data.to_ullong() <= 0xFFFFF)
				pia_enable.set_data(1);
		};
	}
}
