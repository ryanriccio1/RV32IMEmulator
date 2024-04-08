#include "core.h"

#include <iostream>

namespace RV32IM
{
	Core::Core() : Core(0x100000)	{}

	Core::Core(const size_t memory_size) : Core(memory_size, 0, 320, 240)
	{
	}

	Core::Core(const size_t memory_size, const int time_per_clock, const unsigned_data video_width, const unsigned_data video_height) :
		fetch(new Stage::Fetch(this)),
		decode(new Stage::Decode(this)),
		execute(new Stage::Execute(this)),
		memory_stage(new Stage::Memory(this)),
		write_back(new Stage::WriteBack(this)),
		register_file(new RegisterFile()),
		memory(new UnifiedMemory(memory_size)),
		branch(new BranchPrediction(memory_size)),
		video_interface(new VideoInterface(memory, video_width, video_height)),
		video_width(video_width),
		video_height(video_height),
		memory_size(memory_size),
		block_irq(false),
		start(),
		end(),
		desired_clock_time(time_per_clock),
		average_clock_time(0),
		halt_clock(true),
		timer_counter(0),
		stop_counter(true)
	{
	}

	Core::~Core()
	{
		stop_clock();
	}

	void Core::clock() const
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

	void Core::get_irq_free() const
	{
		while (memory->read_byte(irq_handle) != 1) {}
	}

	void Core::load_memory_contents(const shared_ptr<uint8_t[]>& new_memory)
	{
		reset();
        memory->load_memory_contents(new_memory);
		start_clock();
	}

	void Core::set_desired_clock_time(const int time_per_clock)
	{
		desired_clock_time = time_per_clock;
	}

	shared_ptr<uint8_t[]>& Core::get_memory_ptr() const
	{
		return memory->get_memory_ptr();
	}

	shared_ptr<uint8_t[]>& Core::get_video_memory() const
	{
		return video_interface->get_video_memory();
	}

	size_t Core::get_memory_size() const
	{
		return memory_size;
	}

	size_t Core::get_video_width() const
	{
		return video_width;
	}

	size_t Core::get_video_height() const
	{
		return video_height;
	}

	void Core::start_clock()
	{
		if (stop_counter && halt_clock)
		{
			stop_counter = false;
			halt_clock = false;

			video_interface->start_drawing();

			counter_thread = thread([this]()
				{
					while (!stop_counter)
					{
						this_thread::sleep_for(chrono::milliseconds(1));
						timer_counter++;
						memory->write_byte(timer_in, timer_counter);
						if (timer_counter == 255)
							notify_timer();
					}
				});

			clock_thread = thread([this]()
				{
					while (!halt_clock)
					{
						average_clock_time = moving_average.get_average();

						if (desired_clock_time != 0)
							this_thread::sleep_for(chrono::duration_cast<chrono::nanoseconds>(chrono::nanoseconds(desired_clock_time * 512 - average_clock_time * 512)));

						start = chrono::steady_clock::now();
						for (size_t i{ 0 }; i < 512; i++)
							clock();

						end = chrono::steady_clock::now();
						moving_average.add_sample((end - start).count() >> 9);
					}
				});
		}
	}

	void Core::stop_clock()
	{
		if (!stop_counter && !halt_clock)
		{
			video_interface->stop_drawing();

			stop_counter = true;
			if (counter_thread.joinable())
				counter_thread.join();

			halt_clock = true;
			if (clock_thread.joinable())
				clock_thread.join();
		}
	}

	void Core::step_clock() const
	{
		if (stop_counter && halt_clock)
		{
			clock();
			video_interface->start_drawing();
			video_interface->stop_drawing();
		}
	}

	void Core::reset()
	{
		stop_clock();
		fetch = make_unique<Stage::Fetch>(this);
		decode = make_unique < Stage::Decode>(this);
		execute = make_unique<Stage::Execute>(this);
		memory_stage = make_unique<Stage::Memory>(this);
		write_back = make_unique<Stage::WriteBack>(this);
		register_file = make_unique<RegisterFile>();
		branch = make_unique<BranchPrediction>(memory_size);
	}

	void Core::notify_keypress(const char input)
    {
		get_irq_free();
        memory->write_byte(keyboard_in, input);
        memory->write_byte(irq_vector, KEYBOARD);
        interrupt();
    }

	void Core::notify_uart_keypress(const char input)
	{
		get_irq_free();
        memory->write_byte(uart_rx, input);
        memory->write_byte(irq_vector, UART_RX);
        interrupt();
	}

	void Core::notify_timer()
	{
		get_irq_free();
		memory->write_byte(irq_vector, TIMER);
		interrupt();
	}

	unsigned char Core::get_uart() const
	{
        return memory->read_byte(uart_tx);
	}

	unsigned_data Core::get_current_address() const
	{
		return fetch->reg_PC;
	}

	array<unsigned_data, RegisterFile::NUM_REGISTERS>& Core::get_registers() const
	{
		return register_file->get_registers();
	}

	int Core::get_average_clock_time() const
	{
		return average_clock_time;
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
