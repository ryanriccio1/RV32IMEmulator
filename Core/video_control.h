#pragma once
#include <memory>
#include <thread>
#include <vector>

#include "common.h"
#include "unified_memory.h"

namespace RV32IM
{
	using namespace std;
	class VideoInterface
	{
	public:
		VideoInterface(const shared_ptr<UnifiedMemory>& memory, unsigned_data video_width, unsigned_data video_height);
		void start_drawing();
		void stop_drawing();
		shared_ptr<uint8_t[]>& get_video_memory();

	private:
		void draw_bitmap() const;
		void draw_character();

		unsigned_data get_video_memory_address() const;
		unsigned_data get_color_memory_address() const;
		unsigned_data get_char_memory_address() const;

		const unsigned_data video_memory_size;
		shared_ptr<UnifiedMemory> memory;
		shared_ptr<uint8_t[]> video_memory;
		vector<vector<uint32_t>> temp_buffer;

		const unsigned_data video_width;
		const unsigned_data video_height;

		thread draw_thread;
		atomic_bool halt_drawing;
	};
}
