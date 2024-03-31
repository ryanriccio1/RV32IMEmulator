#pragma once

namespace RISCV
{
	using namespace std;
	class VideoInterface
	{
	public:
		VideoInterface();
		VideoInterface(const size_t &width, const size_t &height);
		shared_ptr<uint8_t[]> get_video_memory();
		void set_memory(shared_ptr<uint8_t[]>& memory);
		void update_memory();
	private:
		shared_ptr<uint8_t[]> video_buffer;
		shared_ptr<uint8_t[]> memory;
		size_t width;
		size_t height;
	};

	inline VideoInterface::VideoInterface() : VideoInterface( 320, 240) {	}

	inline VideoInterface::VideoInterface(const size_t &width, const size_t &height) : width(width), height(height)
	{
		video_buffer = make_shared<uint8_t[]>(width * height * 4);
	}

	inline shared_ptr<uint8_t[]> VideoInterface::get_video_memory()
	{
		return video_buffer;
	}

	inline void VideoInterface::set_memory(shared_ptr<uint8_t[]>& new_memory)
	{
		memory = new_memory;
		memcpy_s(video_buffer.get(), width * height * 4, memory.get(), width * height * 4);
	}

	inline void VideoInterface::update_memory()
	{
		memcpy_s(video_buffer.get(), width * height * 4, memory.get()+0x249F0, width * height * 4);
	}
}
