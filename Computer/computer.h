#pragma once

#include "../Core/core.h"
#include "../Memory/unified_memory.h"
#include "../VideoInterface/video_interface.h"

namespace Computer
{
	using namespace std;
	class Computer
	{
	public:
		Computer();
		Computer(const size_t& width, const size_t& height, const size_t& memory_size);
		void load_memory_contents(shared_ptr<uint8_t[]>& new_pointer) const;
		void perform_clock_tick();
		shared_ptr<uint8_t[]> get_video_memory() const;
		void update_screen_size(const size_t& new_width, const size_t& new_height);
		size_t get_memory_size() const;
		size_t get_video_width() const;
		size_t get_video_height() const;
		unique_ptr<RISCV::VideoInterface> video_interface;
		unique_ptr<RISCV::Core<32>> core;


	private:
		unique_ptr<RISCV::UnifiedMemory<32, 4>> memory;

		bitset<1> clock;
		size_t width;
		size_t height;
		size_t memory_size;
	};

	inline Computer::Computer() : Computer(320, 240, 0x10000) {}

	inline Computer::Computer(const size_t& width, const size_t& height, const size_t& memory_size) : width(width), height(height), memory_size(memory_size)
	{
		core = make_unique<RISCV::Core<32>>();
		memory = make_unique<RISCV::UnifiedMemory<32, 4>>(memory_size);
		video_interface = make_unique<RISCV::VideoInterface>(width, height);
		auto memory_ptr = memory->get_memory_ptr();
		video_interface->set_memory(memory_ptr);

		clock = 1;
		core->reset_all();

		core->data_out.on_state_change = [this](std::bitset<32> new_data)
			{
				memory->data_in[0].set_data(new_data);
			};
		core->address.on_state_change = [this](std::bitset<32> new_data)
			{
				memory->address[0].set_data(new_data << 12 >> 12);
			};
		core->memory_write_en.on_state_change = [this](std::bitset<1> new_data)
			{
				memory->enable.set_data(new_data);
			};
		core->data_type.on_state_change = [this](std::bitset<2> new_data)
			{
				memory->data_type[0].set_data(new_data);
			};
		memory->data_out[0].on_state_change = [this](std::bitset<32> new_data)
			{
				core->data_in.set_data(new_data);
			};
	}

	inline void Computer::load_memory_contents(shared_ptr<uint8_t[]>& new_pointer) const
	{
		memory->load_memory_contents(new_pointer);
		video_interface->set_memory(new_pointer);
		core->reset_all();
	}

	inline void Computer::perform_clock_tick()
	{
		core->clock.set_data(clock);
		memory->clock[0].set_data(clock);
		clock.flip();
		//video_interface->update_memory();
	}

	inline shared_ptr<uint8_t[]> Computer::get_video_memory() const
	{
		return video_interface->get_video_memory();
	}

	inline void Computer::update_screen_size(const size_t& new_width, const size_t& new_height)
	{
		width = new_width;
		height = new_height;
		video_interface = make_unique<RISCV::VideoInterface>(width, height);
	}

	inline size_t Computer::get_memory_size() const
	{
		return memory_size;
	}

	inline size_t Computer::get_video_width() const
	{
		return width;
	}

	inline size_t Computer::get_video_height() const
	{
		return height;
	}
}
