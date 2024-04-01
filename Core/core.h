#pragma once

#include "branch.h"
#include "register_file.h"
#include "unified_memory.h"


namespace RV32IM
{
	class RegisterFile;
	class UnifiedMemory;
	class BranchPrediction;

	namespace Stage
	{
		class Fetch;
		class Decode;
		class Execute;
		class Memory;
		class WriteBack;
	}

	class Core
	{
	public:
		friend class Stage::Fetch;
		friend class Stage::Decode;
		friend class Stage::Execute;
		friend class Stage::Memory;
		friend class Stage::WriteBack;
		Core();
		Core(size_t memory_size);
		void clock();
		void load_memory_contents(const shared_ptr<uint8_t[]>& new_memory);
		unsigned_data get_PC();
	private:
		Stage::Fetch* fetch;
		Stage::Decode* decode;
		Stage::Execute* execute;
		Stage::Memory* memory_stage;
		Stage::WriteBack* write_back;

		RegisterFile register_file;
		UnifiedMemory memory;
		BranchPrediction branch;
	};
}
