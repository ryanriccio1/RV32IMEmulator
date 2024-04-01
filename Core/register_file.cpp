#include "register_file.h"

namespace RV32IM
{
	unsigned_data RegisterFile::read(const size_t idx) const
	{
		return idx == 0 ? 0 : outputs[idx];
	}

	auto RegisterFile::write(const size_t idx, unsigned_data value) -> void
	{
		inputs[idx] = value;
	}

	void RegisterFile::clock()
	{
		ranges::copy(inputs, outputs.begin());
	}
}
