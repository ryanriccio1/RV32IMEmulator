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
			bool resetting;
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

		template <size_t control_bits>
		class Decoder
		{
		public:
			Decoder();
			InputPin<1> enable;
			InputPin<control_bits> select;
			array<OutputPin<1>, 1 << control_bits> data_out;
		private:
			void zero_output();
		};

		template <size_t bit_width_in, size_t starting_bit, size_t ending_bit>
		class Splitter
		{
		public:
			Splitter();
			InputPin<bit_width_in> data_in;
			OutputPin<ending_bit - starting_bit + 1> data_out;
		};

		template <size_t bit_width_in, size_t bit_width_out>
		class SignExtend
		{
		public:
			SignExtend();
			InputPin<bit_width_in> data_in;
			OutputPin<bit_width_out> data_out;
			static bitset<bit_width_out> calc_sign_extend(bitset<bit_width_in> new_data);
		};

		template <size_t bit_width_in, size_t bit_width_out>
		class ZeroExtend
		{
		public:
			ZeroExtend();
			InputPin<bit_width_in> data_in;
			OutputPin<bit_width_out> data_out;
			static bitset<bit_width_out> calc_zero_extend(bitset<bit_width_in> new_data);
		};

		template <size_t bit_width>
		class ANDGate
		{
		public:
			ANDGate();
			InputPin<bit_width> a;
			InputPin<bit_width> b;

			OutputPin<bit_width> data_out;
		};

		template <size_t bit_width>
		class ORGate
		{
		public:
			ORGate();
			InputPin<bit_width> a;
			InputPin<bit_width> b;

			OutputPin<bit_width> data_out;
		};

		template <size_t bit_width, size_t max_value>
		class Counter
		{
		public:
			Counter();
			Counter(bool stay_at_value);
			InputPin<1> reset;
			InputPin<1> clock;
			InputPin<1> write_enable;

			OutputPin<1> data_out;
		private:
			size_t count;
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
				if (!resetting)
				{
					bitset<1> clock_val = new_data;
					if (!this->rising_edge)
						clock_val = ~clock_val;
					if ((write_enable.get_data() & clock_val).to_ullong() != 0)
						data_out.set_data(data_in.get_data());
				}
			};
			reset.on_state_change = [this](bitset<1> reset_pin)
			{
				if (reset_pin != 0)
				{
					data_out.set_data(0);
					resetting = true;
				}
				else
					resetting = false;
			};
		}

		template <size_t bit_width, size_t control_bits>
		Multiplexer<bit_width, control_bits>::Multiplexer()
		{
			select.on_state_change = [this](bitset<control_bits> new_data)
				{
					size_t current_idx{ 0 };
					for (InputPin<bit_width>& input_pin : this->data_in)
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

		template <size_t control_bits>
		Decoder<control_bits>::Decoder()
		{
			enable.on_state_change = [this](bitset<1> new_data)
				{
					if (new_data == 0)
						zero_output();
					if (new_data == 1)
						select.set_data(select.get_data());
				};

			select.on_state_change = [this](bitset<control_bits> new_data)
				{
					if (enable.get_data() == 1)
						data_out[exp(2, new_data)].set_data(1);
				};
			select.set_data(0);
		}

		template <size_t control_bits>
		void Decoder<control_bits>::zero_output()
		{
			for (auto output : data_out)
			{
				output.set_data(0);
			}
		}

		template <size_t bit_width_in, size_t starting_bit, size_t ending_bit>
		Splitter<bit_width_in, starting_bit, ending_bit>::Splitter()
		{
			data_in.on_state_change = [this](bitset<bit_width_in> new_data)
				{
					bitset<bit_width_in> bitmask = (static_cast<uint64_t>(1) << (ending_bit + 1)) - 1;
					bitmask >>= starting_bit;
					bitmask <<= starting_bit;
					this->data_out.set_data(((new_data & bitmask) >> starting_bit).to_ullong());
				};
		}

		template <size_t bit_width_in, size_t bit_width_out>
		SignExtend<bit_width_in, bit_width_out>::SignExtend()
		{
			data_in.on_state_change = [this](bitset<bit_width_in> new_data)
				{
					data_out.set_data(calc_sign_extend(new_data));
				};
		}

		template <size_t bit_width_in, size_t bit_width_out>
		bitset<bit_width_out> SignExtend<bit_width_in, bit_width_out>::calc_sign_extend(bitset<bit_width_in> new_data)
		{
			uint64_t data = new_data.to_ullong();
			if (new_data[bit_width_in - 1] == 1)
			{
				const uint64_t bitmask = ((static_cast<uint64_t>(1) << (64 - bit_width_out)) - 1) << bit_width_in;
				data |= bitmask;
			}
			return data;
		}

		template <size_t bit_width_in, size_t bit_width_out>
		ZeroExtend<bit_width_in, bit_width_out>::ZeroExtend()
		{
			data_in.on_state_change = [this](bitset<bit_width_in> new_data)
				{
					data_out.set_data(calc_zero_extend(new_data));
				};
		}

		template <size_t bit_width_in, size_t bit_width_out>
		bitset<bit_width_out> ZeroExtend<bit_width_in, bit_width_out>::calc_zero_extend(bitset<bit_width_in> new_data)
		{
			return new_data.to_ullong();
		}

		template <size_t bit_width>
		ANDGate<bit_width>::ANDGate()
		{
			a.on_state_change = [this](bitset<bit_width> new_data)
				{
					data_out.set_data(a.get_data() & b.get_data());
				};
			b.on_state_change = [this](bitset<bit_width> new_data)
				{
					data_out.set_data(a.get_data() & b.get_data());
				};
		}

		template <size_t bit_width>
		ORGate<bit_width>::ORGate()
		{
			a.on_state_change = [this](bitset<bit_width> new_data)
				{
					data_out.set_data(a.get_data() | b.get_data());
				};
			b.on_state_change = [this](bitset<bit_width> new_data)
				{
					data_out.set_data(a.get_data() | b.get_data());
				};
		}

		template <size_t bit_width, size_t max_value>
		Counter<bit_width, max_value>::Counter() : Counter(true) {}

		template <size_t bit_width, size_t max_value>
		Counter<bit_width, max_value>::Counter(bool stay_at_value) : count(0)
		{
			reset.on_state_change = [this](bitset<1> new_data)
				{
					if (new_data == 1)
					{
						count = 0;
						data_out.set_data(0);
					}
				};
			clock.on_state_change = [this, stay_at_value](bitset<1> new_data)
				{
					if (new_data == 1 && write_enable.get_data() == 1)
					{
						count++;
						if (count == max_value)
							data_out.set_data(1);
						if (count > max_value)
						{
							if (stay_at_value)
								count--;
							else
							{
								count = 0;
								data_out.set_data(0);
							}
						}
					}
				};
		}

		template<size_t bit_width>
		int64_t bitset_to_int(std::bitset<bit_width> bitSet) {
			if (!bitSet[bit_width - 1]) return bitSet.to_ullong();
			bitSet.flip();
			const int64_t new_int = bitSet.to_ullong() + 1;
			return -new_int;
		}
	}
}