#include "branch.h"

namespace RV32IM
{
	BranchPrediction::BranchPrediction() : BranchPrediction(0x100000) {}

	BranchPrediction::BranchPrediction(const size_t& memory_size) : branch_status_table(new uint8_t[memory_size]), branch_history_table(new uint8_t[memory_size]), memory_size(memory_size)
	{
		memset(branch_status_table.get(), 0, sizeof(uint8_t) * memory_size);
		memset(branch_history_table.get(), 0, sizeof(uint8_t) * memory_size);
	}

	bool BranchPrediction::take_branch(unsigned_data address) const
	{
		// 00 strong no branch
		// 01 weak no branch
		// 10 weak branch
		// 11 strong branch
		// uses branch history as factor to adjust branch behavior
		// uses 4 status entries per word (works since all branches must be word boundary aligned)
		address &= (memory_size - 1);
		return branch_status_table[address + (branch_history_table[address] & 0b11)] >= 0b10;
	}

	void BranchPrediction::update_table(unsigned_data address, const bool branch_taken)
	{
		address &= (memory_size - 1);
		const unsigned_data branch_address = address + (branch_history_table[address] & 0b11);
		if (branch_taken) 
		{
			if (branch_status_table[branch_address] < 3) 
				branch_status_table[branch_address]++;
		}
		else 
		{
			if (branch_status_table[branch_address] > 0) 
				branch_status_table[branch_address]--;
		}
		branch_history_table[address] = branch_history_table[address] << 1 | (branch_taken ? 1 : 0);
	}
}
