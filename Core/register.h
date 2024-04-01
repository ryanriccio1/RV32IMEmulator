#pragma once

namespace RV32IM
{
	template <typename T>
	class Register
	{
	public:
		Register() : input(0), output(0), write_enable(true) {}

		Register& operator=(const T& new_value) { write(new_value); return *this; }
		operator T() { return read(); }

		void set_write_enable(const bool new_write_enable) { write_enable = new_write_enable; }
		bool get_write_enable() const { return write_enable; }

		T read() const { return output; }
		T get_input() const { return input; }
		void write(const T& new_value) { input = new_value; }

		void clock()
		{
			if (write_enable)
				output = input;
		}

	private:
		T input;
		T output;
		bool write_enable;
	};
}
