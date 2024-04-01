#include "core.h"

namespace RV32IM
{
	Core::Core() : Core(0x100000)	{}

	Core::Core(const size_t memory_size) :
		fetch(new Stage::Fetch(this)),
		decode(new Stage::Decode(this)),
		execute(new Stage::Execute(this)),
		memory_stage(new Stage::Memory(this)),
		write_back(new Stage::WriteBack(this)),
		register_file(new RegisterFile()),
		memory(new UnifiedMemory(memory_size)),
		branch(new BranchPrediction(memory_size)),
		block_irq(false)
	{
	}

	void Core::clock()
	{
        write_back->run();
        memory_stage->run();
        execute->run();
        decode->run();
        fetch->run();

        fetch->clock();
        decode->clock();
        execute->clock();
        memory_stage->clock();
        write_back->clock();

        register_file->clock();
	}

	void Core::load_memory_contents(const shared_ptr<uint8_t[]>& new_memory) const
	{
        memory->load_memory_contents(new_memory);
	}

    void Core::notify_keypress(const char input)
    {
        memory->write_byte(keyboard_port, input);
        memory->write_byte(irq_vector, 0);
        interrupt();
    }

	void Core::notify_uart_keypress(const char input)
	{
        memory->write_byte(uart_rx, input);
        memory->write_byte(irq_vector, 1);
        interrupt();
	}

	unsigned char Core::get_uart() const
	{
        return memory->read_byte(uart_tx);
	}

	void Core::interrupt()
	{
        if (memory->read_byte(irq_handle) == 1)
        {
            block_irq = false;
            memory->write_byte(irq_handle, 0);
        }
        if (memory->read_byte(irq_en) == 1 && !block_irq)
        {
            block_irq = true;
            decode->irq();
        }
	}
}
