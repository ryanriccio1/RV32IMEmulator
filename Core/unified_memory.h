#pragma once
#include <memory>


namespace RV32IM
{
	using namespace std;
	class UnifiedMemory
	{
	public:
		UnifiedMemory();
		UnifiedMemory(const size_t& memory_size);
		void load_memory_contents(const shared_ptr<uint8_t[]>& new_memory);
		shared_ptr<uint8_t[]>& get_memory_ptr();

		uint32_t read_word(uint32_t address) const;
		uint16_t read_half_word(uint32_t address) const;
		uint8_t read_byte(uint32_t address) const;

		void write_word(uint32_t address, uint32_t data) const;
		void write_half_word(uint32_t address, uint16_t data) const;
		void write_byte(uint32_t address, uint8_t data) const;

		uint8_t& operator[](const uint8_t& address) const
		{
			return memory[address];
		}

	private:
		shared_ptr<uint8_t[]> memory;
		size_t memory_size;
	};

	inline UnifiedMemory::UnifiedMemory() : UnifiedMemory(0x100000)	{}

	inline UnifiedMemory::UnifiedMemory(const size_t& memory_size) : memory(new uint8_t[memory_size]), memory_size(memory_size)
	{
		memset(memory.get(), 0, sizeof(uint8_t) * memory_size);
	}

	inline void UnifiedMemory::load_memory_contents(const shared_ptr<uint8_t[]>& new_memory)
	{
		memory = new_memory;
	}

	inline shared_ptr<uint8_t[]>& UnifiedMemory::get_memory_ptr()
	{
		return memory;
	}

	inline uint32_t UnifiedMemory::read_word(const uint32_t address) const
	{
		return *(reinterpret_cast<uint32_t*>(memory.get() + (address & (memory_size - 1))));
	}

	inline uint16_t UnifiedMemory::read_half_word(const uint32_t address) const
	{
		return *(reinterpret_cast<uint16_t*>(memory.get() + (address & (memory_size - 1))));
	}

	inline uint8_t UnifiedMemory::read_byte(const uint32_t address) const
	{
		return memory[(address & (memory_size - 1))];
	}

	inline void UnifiedMemory::write_word(const uint32_t address, const uint32_t data) const
	{
		*(reinterpret_cast<uint32_t*>(memory.get() + (address & (memory_size - 1)))) = data;
	}

	inline void UnifiedMemory::write_half_word(const uint32_t address, const uint16_t data) const
	{
		*(reinterpret_cast<uint16_t*>(memory.get() + (address & (memory_size - 1)))) = data;
	}

	inline void UnifiedMemory::write_byte(const uint32_t address, const uint8_t data) const
	{
		memory[(address & (memory_size - 1))] = data;
	}
}
