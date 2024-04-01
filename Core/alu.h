#pragma once

#include <cassert>

#include "common.h"
#include "instruction.h"

namespace RV32IM
{
	class ALU
	{
	public:
		static unsigned_data get_result(const Instruction& instruction, unsigned_data rs1, unsigned_data rs2, unsigned_data pc);
	private:
		static unsigned_data get_calculation(const Instruction& instruction, unsigned_data rs1, unsigned_data rs2, unsigned_data pc=0x0);
	};

	
}
