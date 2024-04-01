#include "register_file.h"

namespace RV32IM
{
	unsigned_data RegisterFile::read(const RegisterName reg) const
	{
		return reg == 0 ? 0 : outputs[reg];
	}

	auto RegisterFile::write(const RegisterName reg, const unsigned_data value) -> void
	{
		inputs[reg] = value;
	}

	void RegisterFile::clock()
	{
		ranges::copy(inputs, outputs.begin());
	}
}
