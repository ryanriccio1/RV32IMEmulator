#include "core.h"

#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"
#include "write_back.h"

namespace RV32IM
{
	Core::Core() : Core(0x100000)	{}

	Core::Core(size_t memory_size)
	{
        fetch = new Stage::Fetch(this);
        decode = new Stage::Decode(this);
        execute = new Stage::Execute(this);
        memory_stage = new Stage::Memory(this);
        write_back = new Stage::WriteBack(this);
        branch = BranchPrediction(memory_size);
        memory = UnifiedMemory(memory_size);
        register_file = RegisterFile();
	}

	void Core::clock()
	{
        write_back->run();
        memory_stage->run();
        execute->run();
        decode->run();
        fetch->run();

        fetch->clock();
        decode->clock();
        execute->clock();
        memory_stage->clock();
        write_back->clock();

        register_file.clock();
	}

	void Core::load_memory_contents(const shared_ptr<uint8_t[]>& new_memory)
	{
        memory.load_memory_contents(new_memory);
	}

	unsigned_data Core::get_PC()
	{
        return fetch->reg_PC;
	}
}
