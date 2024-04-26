#include "common.h"

#include <cassert>

namespace RV32IM
{
	constexpr unsigned_data generate_bitmask(size_t bit_width)
	{
		assert(bit_width <= 32);
		return (1 << bit_width) - 1;
	}

	unsigned_data mask_data(unsigned_data data, size_t low_bit, size_t high_bit)
	{
		assert(high_bit >= low_bit);
		return (data >> low_bit) & generate_bitmask(high_bit - low_bit + 1);
	}

	unsigned_data sign_extend(unsigned_data data, size_t space)
	{
		assert(data <= 1);
		assert(space <= 32);
		return data ? (0xFFFFFFFF << space) : 0;
	}

	void update_offsets(const uint32_t memory_size)
	{
		vga_mode = memory_size - offset_vga_mode;
		vga_mem_ptr = memory_size - offset_vga_mem_ptr;
		col_mem_ptr = memory_size - offset_col_mem_ptr;
		chr_mem_ptr = memory_size - offset_chr_mem_ptr;

		irq_mask = memory_size - offset_irq_mask;
		timer_in = memory_size - offset_timer_in;
		keyboard_in = memory_size - offset_keyboard_in;
		uart_tx = memory_size - offset_uart_tx;
		uart_rx = memory_size - offset_uart_rx;
		irq_en = memory_size - offset_irq_en;
		irq_handle = memory_size - offset_irq_handle;
		irq_vector = memory_size - offset_irq_vector;
		data_ready = memory_size - offset_data_ready;
	}
}
