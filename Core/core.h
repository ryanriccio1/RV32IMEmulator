#pragma once

#include <chrono>
#include <thread>
#include <string>

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
		// explicit Core(size_t memory_size);
		Core(int time_per_clock, int video_width, int video_height);
		~Core();

		void load_memory_contents(const shared_ptr<uint8_t[]>& new_memory, size_t new_memory_size);
		void set_desired_clock_time(int time_per_clock);

		[[nodiscard]] shared_ptr<uint8_t[]>& get_memory_ptr() const;
		[[nodiscard]] shared_ptr<uint8_t[]>& get_video_memory() const;
		[[nodiscard]] size_t get_memory_size() const;
		[[nodiscard]] int get_video_width() const;
		[[nodiscard]] int get_video_height() const;

		void start_clock();
		void stop_clock();
		void step_clock();
		void reset();

		void notify_keypress(unsigned char input);
		void notify_uart_keypress(unsigned char input);
		void notify_timer();

		[[nodiscard]] bool is_clock_running() const;
		[[nodiscard]] unsigned_data get_current_address() const;
		[[nodiscard]] array<unsigned_data, RegisterFile::NUM_REGISTERS>& get_registers() const;
		[[nodiscard]] int get_average_clock_time() const;
		[[nodiscard]] int get_average_processing_time() const;
		[[nodiscard]] bool get_irq() const;
		[[nodiscard]] string get_uart_data() const;

	private:
		void interrupt();
		void clock() const;
		void get_irq_free() const;
		void start_uart_tx();
		void stop_uart_tx();

		unique_ptr<Stage::Fetch> fetch;
		unique_ptr<Stage::Decode> decode;
		unique_ptr<Stage::Execute> execute;
		unique_ptr<Stage::Memory> memory_stage;
		unique_ptr<Stage::WriteBack> write_back;

		unique_ptr<RegisterFile> register_file;
		shared_ptr<UnifiedMemory> memory;
		unique_ptr<BranchPrediction> branch;

		unique_ptr<VideoInterface> video_interface;
		int video_width;
		int video_height;
		size_t memory_size;

		bool block_irq;

		MovingAverage<int, 32> average_clock;
		MovingAverage<int, 32> average_processing;
		chrono::time_point<chrono::steady_clock> clock_start;
		chrono::time_point<chrono::steady_clock> processing_start;
		chrono::time_point<chrono::steady_clock> end;
		int desired_clock_time;

		thread clock_thread;
		bool halt_clock;

		uint8_t timer_counter;
		thread counter_thread;
		bool halt_counter;

		string uart_data;
		thread uart_tx_thread;
		bool halt_uart;
	};

	
}
