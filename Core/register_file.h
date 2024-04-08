#pragma once
#include <array>

#include "common.h"

namespace RV32IM
{
	class RegisterFile
	{
	public:
		static constexpr size_t NUM_REGISTERS = 32;
		RegisterFile() = default;

		unsigned_data read(RegisterName reg) const;
		void write(RegisterName reg, unsigned_data value);

		void clock();
		array<unsigned_data, NUM_REGISTERS>& get_registers();
	private:
		array<unsigned_data, NUM_REGISTERS> inputs;
		array<unsigned_data, NUM_REGISTERS> outputs;
	};
}
