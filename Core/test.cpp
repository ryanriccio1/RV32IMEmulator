#include <iostream>

#include "../Core/core.h"
#include "../Memory/unified_memory.h"

int main()
{
	auto core = new RISCV::Core<32>();
	auto memory = new RISCV::UnifiedMemory<32, 1000000, 4>();
}