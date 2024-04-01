#include "branch.h"

namespace RV32IM
{
	BranchPrediction::BranchPrediction() : BranchPrediction(0x100000) {}

	BranchPrediction::BranchPrediction(const size_t& memory_size) : two_bits(new uint8_t[memory_size]), mux(new uint8_t[memory_size])
	{
		memset(two_bits.get(), 0, sizeof(uint8_t) * memory_size);
		memset(mux.get(), 0, sizeof(uint8_t) * memory_size);
	}

	// need a better understanding of this
	bool BranchPrediction::take_branch(unsigned_data address) const
	{
		return two_bits[address + (mux[address] & 0x3)] >= 2;
	}

	void BranchPrediction::update_table(unsigned_data address, bool branch_taken)
	{
		const unsigned_data branch_address = address + (mux[address] & 0x3);
		if (branch_taken) 
		{
			if (two_bits[branch_address] < 3) two_bits[branch_address]++;
		}
		else 
		{
			if (two_bits[branch_address] > 0) two_bits[branch_address]--;
		}
		mux[address] = mux[address] << 1 | (branch_taken ? 1 : 0);
	}
}
