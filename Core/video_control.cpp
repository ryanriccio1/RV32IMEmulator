#include "video_control.h"

#include <ranges>

namespace RV32IM
{
	VideoInterface::VideoInterface(const shared_ptr<UnifiedMemory>& memory, const unsigned_data video_width, const unsigned_data video_height) :
		video_memory_size(video_width * video_height * 4),
		memory(memory),
		video_memory(new uint8_t[video_memory_size]),
		temp_buffer(video_height),
		video_width(video_width),
		video_height(video_height),
		halt_drawing(true)
	{
		for (unsigned_data y{ 0 }; y < video_height; y++)
		{
			temp_buffer[y] = vector<uint32_t>(video_width);
		}
	}

	void VideoInterface::start_drawing()
	{
		if (halt_drawing)
		{
			halt_drawing = false;

			draw_thread = thread([this]()
				{
					while (!halt_drawing)
					{
						switch (memory->read_byte(vga_mode))
						{
						default:
						case BITMAP:
							draw_bitmap();
							break;
						case CHARACTER:
							draw_character();
							break;
						}
					}
				});


		}
	}

	void VideoInterface::stop_drawing()
	{
		if (!halt_drawing)
		{
			halt_drawing = true;
			if (draw_thread.joinable())
				draw_thread.join();
		}
	}

	shared_ptr<uint8_t[]>& VideoInterface::get_video_memory()
	{
		return video_memory;
	}

	void VideoInterface::draw_bitmap() const
	{
		memcpy_s(video_memory.get(), video_memory_size, memory->get_memory_ptr().get() + get_video_memory_address(), video_memory_size);
	}

	void VideoInterface::draw_character()
	{
		const unsigned_data video_mem_address = get_video_memory_address();
		const unsigned_data color_mem_address = get_color_memory_address();
		const unsigned_data char_mem_address = get_char_memory_address();


		for (unsigned_data y{ 0 }; y < video_height; y++)
		{
			for (unsigned_data x{ 0 }; x < video_width; x+=8)
			{
				const unsigned_data select_character = memory->read_word(video_mem_address + (x >> 1) + ((y >> 3) * (video_width >> 1)));
				const unsigned_data foreground = memory->read_word(color_mem_address + x + video_width * (y >> 3));
				const unsigned_data background = memory->read_word(color_mem_address + x + 4 + video_width * (y >> 3));
				const uint8_t character_mask = memory->read_byte(char_mem_address + select_character * 8 + (7 - (y % 8)));

				for (unsigned_data i{ 0 }; i < 8; i++)
					temp_buffer[y][i + x] = ((character_mask >> (7 - i)) & 0b00000001) == 1 ? background : foreground;
			}
		}
		ranges::copy(ranges::join_view(temp_buffer), reinterpret_cast<uint32_t*>(video_memory.get()));
	}

	unsigned_data VideoInterface::get_video_memory_address() const
	{
		return memory->read_word(vga_mem_ptr);
	}

	unsigned_data VideoInterface::get_color_memory_address() const
	{
		return memory->read_word(col_mem_ptr);
	}

	unsigned_data VideoInterface::get_char_memory_address() const
	{
		return memory->read_word(chr_mem_ptr);
	}
}