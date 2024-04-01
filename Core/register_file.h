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

		unsigned_data read(size_t idx) const;
		void write(size_t idx, unsigned_data value);

		void clock();

	private:
		array<unsigned_data, NUM_REGISTERS> inputs;
		array<unsigned_data, NUM_REGISTERS> outputs;
	};
}
