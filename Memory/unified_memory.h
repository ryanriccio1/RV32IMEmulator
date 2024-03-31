#pragma once
#include <cstdint>
#include <memory>

#include "../Core/utils.h"
#include "../VideoInterface/video_interface.h"

namespace RISCV
{
	using namespace std;

	template <size_t bit_width, size_t num_data_channels>
	class UnifiedMemory
	{
	public:
		friend VideoInterface;
		UnifiedMemory();
		UnifiedMemory(const size_t& memory_size);
		array<CoreUtils::InputPin<bit_width>, num_data_channels> address;
		array<CoreUtils::InputPin<bit_width>, num_data_channels> data_in;
		array<CoreUtils::InputPin<2>, num_data_channels> data_type;
		array<CoreUtils::InputPin<1>, num_data_channels> clock;
		CoreUtils::InputPin<1> enable;

		array<CoreUtils::OutputPin<bit_width>, num_data_channels> data_out;

		void load_memory_contents(shared_ptr<uint8_t[]>& new_memory);
		shared_ptr<uint8_t[]> get_memory_ptr();
		//void* get_raw_pointer() const;
	private:
		size_t memory_size;
		shared_ptr<uint8_t[]> memory;

		void write_memory(size_t idx);
		bitset<bit_width> get_memory(size_t idx);
		static void bitset_to_byte_array(array<uint8_t, bit_width / 8>& byte_array, bitset<bit_width> data);
	};

	template <size_t bit_width, size_t num_data_channels>
	UnifiedMemory<bit_width, num_data_channels>::UnifiedMemory() : UnifiedMemory(0x100000) {}

	template <size_t bit_width, size_t num_data_channels>
	UnifiedMemory<bit_width, num_data_channels>::UnifiedMemory(const size_t& memory_size) : memory_size(memory_size), memory(new uint8_t[memory_size])
	{
		for (size_t i{ 0 }; i < num_data_channels; i++)
		{
			clock[i].on_state_change = [this, i](bitset<1> new_data)
				{
					if (new_data == 0 && enable.get_data() == 1)
						write_memory(i);
				};
		}
		for (size_t i{ 0 }; i < num_data_channels; i++)
		{
			address[i].on_state_change = [this, i](bitset<bit_width> new_data)
				{
					data_out[i].set_data(get_memory(i));
				};
		}
	}

	template <size_t bit_width, size_t num_data_channels>
	void UnifiedMemory<bit_width, num_data_channels>::load_memory_contents(shared_ptr<uint8_t[]>& new_memory)
	{
		memory = new_memory;
	}

	template <size_t bit_width, size_t num_data_channels>
	shared_ptr<uint8_t[]> UnifiedMemory<bit_width, num_data_channels>::get_memory_ptr()
	{
		return memory;
	}

	/*template <size_t bit_width, size_t len, size_t num_data_channels>
	void* UnifiedMemory<bit_width, len, num_data_channels>::get_raw_pointer() const
	{
		return memory.get();
	}*/

	template <size_t bit_width, size_t num_data_channels>
	void UnifiedMemory<bit_width, num_data_channels>::write_memory(size_t idx)
	{
		array<uint8_t, bit_width / 8> byte_array;
		bitset_to_byte_array(byte_array, data_in[idx].get_data());
		auto data_to_write = byte_array.data();
		auto address_base = address[idx].get_data().to_ullong();
		switch (data_type[idx].get_data().to_ullong())
		{
		default:
		case 0b10:	// word
			memory[address_base + 3] = data_to_write[3];
			memory[address_base + 2] = data_to_write[2];
			[[fallthrough]];
		case 0b01:	// half-word
			memory[address_base + 1] = data_to_write[1];
			[[fallthrough]];
		case 0b00:	// byte
			memory[address_base] = data_to_write[0];
		}
	}

	template <size_t bit_width, size_t num_data_channels>
	bitset<bit_width> UnifiedMemory<bit_width, num_data_channels>::get_memory(size_t idx)
	{
		auto address_base = address[idx].get_data().to_ullong();
		uint64_t accumulator{ 0 };
		for (int i{ bit_width / 8 - 1}; i >= 0; --i)
		{
			accumulator <<= 8;
			accumulator |= memory[address_base + i];
		}
		bitset<bit_width> new_data{ accumulator };
		return new_data;
	}

	template <size_t bit_width, size_t num_data_channels>
	void UnifiedMemory<bit_width, num_data_channels>::bitset_to_byte_array(
		array<uint8_t, bit_width / 8>& byte_array,
		bitset<bit_width> data)
	{	// least significant byte in the lowest memory position
		for (size_t i { 0 }; i < bit_width / 8; i++)
		{
			const bitset<bit_width> bitmask{ 0b11111111 };
			byte_array[i] = static_cast<uint8_t>(((data & (bitmask << (i * 8))) >> i * 8).to_ullong());
		}
	}
}
