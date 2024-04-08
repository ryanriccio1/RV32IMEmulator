#pragma once

#include <chrono>
#include <thread>

#include "branch.h"
#include "decode.h"
#include "execute.h"
#include "fetch.h"
#include "memory.h"
#include "moving_average.h"
#include "register_file.h"
#include "unified_memory.h"
#include "video_control.h"
#include "write_back.h"

namespace RV32IM
{
	class RegisterFile;
	class UnifiedMemory;
	class BranchPrediction;

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
		Core(size_t memory_size, const int time_per_clock, const unsigned_data video_width, const unsigned_data video_height);
		~Core();

		void load_memory_contents(const shared_ptr<uint8_t[]>& new_memory);
		void set_desired_clock_time(const int time_per_clock);

		shared_ptr<uint8_t[]>& get_memory_ptr() const;
		shared_ptr<uint8_t[]>& get_video_memory() const;
		size_t get_memory_size() const;
		size_t get_video_width() const;
		size_t get_video_height() const;

		void start_clock();
		void stop_clock();
		void step_clock() const;
		void reset();

		void notify_keypress(char input);
		void notify_uart_keypress(char input);
		void notify_timer();
		unsigned char get_uart() const;

		unsigned_data get_current_address() const;
		array<unsigned_data, RegisterFile::NUM_REGISTERS>& get_registers() const;
		int get_average_clock_time() const;

	private:
		void interrupt();
		void clock() const;
		void get_irq_free() const;

		unique_ptr<Stage::Fetch> fetch;
		unique_ptr<Stage::Decode> decode;
		unique_ptr<Stage::Execute> execute;
		unique_ptr<Stage::Memory> memory_stage;
		unique_ptr<Stage::WriteBack> write_back;

		unique_ptr<RegisterFile> register_file;
		shared_ptr<UnifiedMemory> memory;
		unique_ptr<BranchPrediction> branch;

		unique_ptr<VideoInterface> video_interface;
		size_t video_width;
		size_t video_height;
		size_t memory_size;

		bool block_irq;

		MovingAverage<int, 32> moving_average;
		chrono::time_point<chrono::steady_clock> start;
		chrono::time_point<chrono::steady_clock> end;
		int desired_clock_time;
		int average_clock_time;

		thread clock_thread;
		bool halt_clock;

		uint8_t timer_counter;
		thread counter_thread;
		bool stop_counter;
	};

	
}
