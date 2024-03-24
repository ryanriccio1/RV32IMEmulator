#include <fstream>
#include <thread>
#include <chrono>
#include <iostream>
#include <format>

#include "../Core/core.h"
#include "../Memory/unified_memory.h"

int main()
{
	std::ifstream file("test_prg.bin", std::ios::binary);
	file.seekg(0, std::ios::end);
	const std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	auto mem_contents = std::unique_ptr<uint8_t[]>(new uint8_t[1048575]);
	memset(mem_contents.get(), 0, 1048575);
	file.read(reinterpret_cast<char*>(mem_contents.get()), size);
	file.close();

	const auto core = new RISCV::Core<32>();
	const auto memory = new RISCV::UnifiedMemory<32, 1048575, 4>();
	memory->load_memory_contents(mem_contents);

	core->data_out.on_state_change = [memory](std::bitset<32> new_data)
		{
			memory->data_in[0].set_data(new_data);
		};
	core->address.on_state_change = [memory](std::bitset<32> new_data)
		{
			memory->address[0].set_data(new_data << 12 >> 12);
		};
	core->memory_write_en.on_state_change = [memory](std::bitset<1> new_data)
		{
			memory->enable.set_data(new_data);
		};
	core->data_type.on_state_change = [memory](std::bitset<2> new_data)
		{
			memory->data_type[0].set_data(new_data);
		};
	memory->data_out[0].on_state_change = [core](std::bitset<32> new_data)
		{
			core->data_in.set_data(new_data);
		};

	core->reset.set_data(1);
	core->reset.set_data(0);
	std::bitset<1> clock = 1;
	size_t counter = 0;
	// Define the time interval (1 second in milliseconds)
	constexpr std::chrono::milliseconds interval(1);
	std::cout << "Start!\n";

	while (true) {
		counter++;
		// Perform the task
		core->clock.set_data(clock);
		memory->clock[0].set_data(clock);

		if (counter == 39100)
			break;
		//std::cout << std::format("{:<4}{:#010x}\t{:#010x}", counter, core->address.get_data().to_ullong(), core->data_in.get_data().to_ullong()) << '\r';
		clock.flip();

		// Sleep the thread for the interval using std::this_thread::sleep_for
		//std::this_thread::sleep_for(interval);
	}
	std::cout << "Done!\n";

}