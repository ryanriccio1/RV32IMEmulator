#pragma once

#include "branch.h"
#include "register_file.h"
#include "unified_memory.h"
#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"
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
		void clock();
		void load_memory_contents(const shared_ptr<uint8_t[]>& new_memory) const;

		void notify_keypress(char input);
		void notify_uart_keypress(char input);
		unsigned char get_uart() const;

	private:
		void interrupt();

		static constexpr unsigned_data keyboard_port = 0xFFFF0;
		static constexpr unsigned_data uart_tx = 0xFFFF1;
		static constexpr unsigned_data uart_rx = 0xFFFF2;
		static constexpr unsigned_data irq_en = 0xFFFF3;
		static constexpr unsigned_data irq_handle = 0xFFFF4;
		static constexpr unsigned_data irq_vector = 0xFFFF5;

		static constexpr unsigned_data vga_mode = 0xFFF00;
		static constexpr unsigned_data vga_mem_ptr = 0xFFF04;
		static constexpr unsigned_data col_mem_ptr = 0xFFF08;
		static constexpr unsigned_data chr_mem_ptr = 0xFFF0C;

		unique_ptr<Stage::Fetch> fetch;
		unique_ptr<Stage::Decode> decode;
		unique_ptr<Stage::Execute> execute;
		unique_ptr<Stage::Memory> memory_stage;
		unique_ptr<Stage::WriteBack> write_back;

		unique_ptr<RegisterFile> register_file;
		unique_ptr<UnifiedMemory> memory;
		unique_ptr<BranchPrediction> branch;

		bool block_irq;
	};

	
}
