#pragma once
#include <bitset>
#include <array>
#include <functional>


namespace RISCV
{
	namespace CoreUtils
	{
		using namespace std;

		template <size_t bit_width>
		class Pin
		{
		public:
			Pin();
			Pin(bitset<bit_width> data);
			void set_data(bitset<bit_width> new_data);
			bitset<bit_width> get_data();
		protected:
			bitset<bit_width> data;
		};

		

		template <size_t bit_width>
		class OutputPin : public Pin<bit_width>
		{
		public:
			void set_data(bitset<bit_width> new_data);
			function<void(bitset<bit_width>)> on_state_change;
		};

		template <size_t bit_width>
		using InputPin = OutputPin<bit_width>;

		template <size_t bit_width>
		class Register
		{
		public:
			Register();
			Register(bool rising_edge);
			InputPin<bit_width> data_in;
			InputPin<1> write_enable;
			InputPin<1> clock;
			InputPin<1> reset;
			OutputPin<bit_width> data_out;
		private:
			bool rising_edge;
		};

		template <size_t bit_width, size_t control_bits>
		class Multiplexer
		{
		public:
			Multiplexer();
			array<InputPin<bit_width>, 1 << control_bits> data_in;
			InputPin<control_bits> select;
			OutputPin<bit_width> data_out;
		};

		template <size_t bit_width_in, size_t starting_bit, size_t ending_bit>
		class Splitter
		{
		public:
			Splitter();
			InputPin<bit_width_in> data_in;
			OutputPin<ending_bit - starting_bit + 1> data_out;
		};

		template <size_t bit_width>
		Pin<bit_width>::Pin() : data(0) {}

		template <size_t bit_width>
		Pin<bit_width>::Pin(bitset<bit_width> data) : data(data) {}

		template <size_t bit_width>
		void Pin<bit_width>::set_data(bitset<bit_width> new_data) { data = new_data; }

		template <size_t bit_width>
		bitset<bit_width> Pin<bit_width>::get_data() { return data; }

		template <size_t bit_width>
		void OutputPin<bit_width>::set_data(bitset<bit_width> new_data)
		{
			this->data = new_data;
			if (on_state_change)
				on_state_change(new_data);
		}

		template <size_t bit_width>
		Register<bit_width>::Register() : Register(true) {}

		template <size_t bit_width>
		Register<bit_width>::Register(bool rising_edge) : rising_edge(rising_edge)
		{
			clock.on_state_change = [this](bitset<1> new_data)
				{
					bitset<1> clock_val = new_data;
					if (!this->rising_edge)
						clock_val = ~clock_val;
					if ((this->write_enable.get_data() & clock_val).to_ullong() != 0)
						this->data_out.set_data(this->data_in.get_data());
				};
			reset.on_state_change = [this](bitset<1> reset_pin)
				{
					if (reset_pin != 0)
						this->data_out.set_data(0);
				};
		}

		template <size_t bit_width, size_t control_bits>
		Multiplexer<bit_width, control_bits>::Multiplexer()
		{
			select.on_state_change = [this](bitset<control_bits> new_data)
				{
					size_t current_idx{ 0 };
					for(InputPin<bit_width> &input_pin : this->data_in)
					{
						if (current_idx == new_data.to_ullong())
						{
							input_pin.on_state_change = [this](bitset<bit_width> dat)
								{
									this->data_out.set_data(dat);
								};
						}
						else
						{
							input_pin.on_state_change = [this](bitset<bit_width>) {};
						}
						current_idx++;
					}
					this->data_out.set_data(this->data_in[new_data.to_ullong()].get_data());
				};
			select.set_data(0);

		}

		template <size_t bit_width_in, size_t starting_bit, size_t ending_bit>
		Splitter<bit_width_in, starting_bit, ending_bit>::Splitter()
		{
			data_in.on_state_change = [this](bitset<bit_width_in> new_data)
				{
					bitset<bit_width_in> bitmask = (1 << (ending_bit + 1)) - 1;
					bitmask >>= starting_bit;
					bitmask <<= starting_bit;
					this->data_out.set_data(((new_data & bitmask) >> starting_bit).to_ullong());
				};
		}

		/*template <size_t bit_width>
		Register<bit_width>::Register() : write_enable(false), clock(false), rising_edge(true) {}

		template <size_t bit_width>
		Register<bit_width>::Register(bool rising_edge) : write_enable(false), clock(false), rising_edge(rising_edge) {}

		template <size_t bit_width>
		Register<bit_width>::Register(bool rising_edge, bool write_enable) : write_enable(write_enable), clock(false), rising_edge(rising_edge) {}

		template <size_t bit_width>
		bitset<bit_width> Register<bit_width>::get_data() {	return data; }

		template <size_t bit_width>
		void Register<bit_width>::set_data(bitset<bit_width> new_data) { data_input = new_data; }

		template <size_t bit_width>
		void Register<bit_width>::set_clock(bool clock_value)
		{
			if (!rising_edge)
				clock_value = ~clock_value;
			clock = clock_value;
			if (write_enable && clock)
				data = data_input;
		}*/
	}
}