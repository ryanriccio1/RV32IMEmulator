#include "common.h"

#include <cassert>

namespace RV32IM
{
	constexpr unsigned_data generate_bitmask(size_t bit_width)
	{
		assert(bit_width <= 32);
		return (1 << bit_width) - 1;
	}

	unsigned_data mask_data(unsigned_data data, size_t low_bit, size_t high_bit)
	{
		assert(high_bit >= low_bit);
		return (data >> low_bit) & generate_bitmask(high_bit - low_bit + 1);
	}

	unsigned_data sign_extend(unsigned_data data, size_t space)
	{
		assert(data <= 1);
		assert(space <= 32);
		return data ? (0xFFFFFFFF << space) : 0;
	}
}
