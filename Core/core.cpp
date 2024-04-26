#include "core.h"

namespace RV32IM
{
	Core::Core() : Core(0, 320, 240)	{}

	/*Core::Core(const size_t memory_size) : Core(memory_size, 0, 320, 240)
	{
	}*/

	Core::Core(const int time_per_clock, const int video_width, const int video_height) :
		fetch(new Stage::Fetch(this)),
		decode(new Stage::Decode(this)),
		execute(new Stage::Execute(this)),
		memory_stage(new Stage::Memory(this)),
		write_back(new Stage::WriteBack(this)),
		register_file(new RegisterFile()),
		memory(new UnifiedMemory(0x100)),
		branch(new BranchPrediction(0x100)),
		video_interface(new VideoInterface(memory, video_width, video_height)),
		video_width(video_width),
		video_height(video_height),
		memory_size(0x100),
		block_irq(false),
		clock_start(),
		processing_start(),
		end(),
		desired_clock_time(time_per_clock),
		halt_clock(true),
		timer_counter(0),
		halt_counter(true),
		halt_uart(true)
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
#ifndef _DEBUG
		if (memory->read_byte(irq_en) == 1)
			while (memory->read_byte(irq_handle) != 1) {}
#endif
	}

	void Core::load_memory_contents(const shared_ptr<uint8_t[]>& new_memory, const size_t new_memory_size)
	{
		bool restart_clock = false;
		if (is_clock_running())
			restart_clock = true;

		stop_clock();
		memory_size = new_memory_size;
		update_offsets(memory_size);
		memory = make_shared<UnifiedMemory>(memory_size);
        memory->load_memory_contents(new_memory);
		reset();

		if (restart_clock)
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

	int Core::get_video_width() const
	{
		return video_width;
	}

	int Core::get_video_height() const
	{
		return video_height;
	}

	void Core::start_clock()
	{
		if (halt_counter && halt_clock)
		{
			halt_counter = false;
			halt_clock = false;

			video_interface->start_drawing();

			counter_thread = thread([this]()
				{
					while (!halt_counter)
					{
						this_thread::sleep_for(chrono::milliseconds(1));
						timer_counter++;
						memory->write_byte(timer_in, timer_counter);
						if (timer_counter % 16 == 0)
							notify_timer();
					}
				});

			clock_thread = thread([this]()
				{
					while (!halt_clock)
					{
						clock_start = chrono::steady_clock::now();

						if (desired_clock_time != 0)
						{
							this_thread::sleep_for(chrono::nanoseconds(desired_clock_time * 512 * 512 - average_processing.get_average() * 512 * 512));
						}
						for (size_t j{ 0 }; j < 512; j++)
						{
							processing_start = chrono::steady_clock::now();
							for (size_t i{ 0 }; i < 512; i++)
								clock();

							end = chrono::steady_clock::now();

							average_processing.add_sample((end - processing_start).count() >> 9);  // NOLINT(clang-diagnostic-shorten-64-to-32, bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
						}
						if (memory->read_byte(irq_handle) == 1)
						{
							block_irq = false;
						}
						average_clock.add_sample((end - clock_start).count() >> 18);  // NOLINT(clang-diagnostic-shorten-64-to-32, bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
					}
				});

			start_uart_tx();
		}
	}

	void Core::stop_clock()
	{
		if (!halt_counter && !halt_clock)
		{
			video_interface->stop_drawing();

			halt_counter = true;
			if (counter_thread.joinable())
				counter_thread.join();

			halt_clock = true;
			if (clock_thread.joinable())
				clock_thread.join();

			stop_uart_tx();
		}
	}

	void Core::step_clock()
	{
		if (halt_counter && halt_clock)
		{
			clock();
			video_interface->start_drawing();
			video_interface->stop_drawing();
			start_uart_tx();
			stop_uart_tx();
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
		video_interface = make_unique<VideoInterface>(memory, video_width, video_height);
		timer_counter = 0;
		block_irq = false;
		uart_data = "";
	}

	void Core::start_uart_tx()
	{
		halt_uart = false;
		uart_tx_thread = thread([this]()
			{
				while (!halt_uart)
				{
					if (memory->read_byte(data_ready) == 1)
					{
						size_t pos;
						const auto tx_data = static_cast<char>(memory->read_byte(uart_tx));
						switch (tx_data)
						{
						case 2:
							uart_data = "";
							break;
						case '\r':
							pos = uart_data.rfind('\n');

							// If the character is found, create a substring excluding the trailing characters
							if (pos != std::string::npos) {
								uart_data = uart_data.substr(0, pos + 1);
							}
							else
							{
								uart_data = "";
							}
							break;
						case '\b':
							if (uart_data.length() > 0)
								uart_data.pop_back();
							break;
						default:
							uart_data += tx_data;
							break;
						}
						memory->write_byte(data_ready, 0);
					}
				}
			});
	}

	void Core::stop_uart_tx()
	{
		halt_uart = true;
		if (uart_tx_thread.joinable())
			uart_tx_thread.join();
	}

	void Core::notify_keypress(const unsigned char input)
    {
		get_irq_free();
        memory->write_byte(keyboard_in, input);
        memory->write_byte(irq_vector, KEYBOARD);
        interrupt();
    }

	void Core::notify_uart_keypress(const unsigned char input)
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

	bool Core::is_clock_running() const
	{
		return !halt_clock;
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
		return average_clock.get_average();
	}

	int Core::get_average_processing_time() const
	{
		return average_processing.get_average();
	}

	bool Core::get_irq() const
	{
		return block_irq;
	}

	string Core::get_uart_data() const
	{
		return uart_data;
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
