#pragma once
#include <memory>

#include "common.h"

namespace RV32IM
{
	class BranchPrediction
	{
	public:
		BranchPrediction();
		BranchPrediction(const size_t& memory_size);
		bool take_branch(unsigned_data address) const;
		void update_table(unsigned_data address, bool branch_taken);
	private:
		unique_ptr<uint8_t[]> branch_status_table;
		unique_ptr<uint8_t[]> branch_history_table;
	};
}
