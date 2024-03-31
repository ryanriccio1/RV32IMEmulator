#pragma once
#include <map>

#include "../Core/utils.h"

namespace RISCV
{
	using namespace std;

	template <size_t bit_width>
	class PeripheralInterfaceAdapter
	{
	public:
		CoreUtils::InputPin<bit_width> address;
		CoreUtils::InputPin<bit_width> data_in;
		CoreUtils::InputPin<1> reset;
		CoreUtils::InputPin<1> en;

		CoreUtils::OutputPin<bit_width> data_out;
		CoreUtils::OutputPin<1> irq;

		void set_key_map(const map<int, bool> new_keyboard);
		void set_uart_char(const char uart_in);
	private:
		map<int, bool> keyboard;
	};

	template <size_t bit_width>
	void PeripheralInterfaceAdapter<bit_width>::set_key_map(const map<int, bool> new_keyboard)
	{
		keyboard = new_keyboard;
	}


	//template <size_t bit_width>
	//class PeripheralInterfaceAdapter
	//{
	//public:
	//	PeripheralInterfaceAdapter();
	//	CoreUtils::InputPin<1> clock;
	//	CoreUtils::InputPin<1> reset;
	//	CoreUtils::InputPin<bit_width> data_in;
	//	CoreUtils::InputPin<bit_width> address;
	//	CoreUtils::OutputPin<bit_width> data_out;

	//	CoreUtils::InputPin<8> port_a_in;
	//	CoreUtils::OutputPin<8> port_a_out;

	//	CoreUtils::InputPin<8> port_b_in;
	//	CoreUtils::OutputPin<8> port_b_out;

	//	CoreUtils::InputPin<8> uart_in;
	//	CoreUtils::OutputPin<8> uart_out;

	//	CoreUtils::InputPin<1> write;
	//	CoreUtils::InputPin<1> read;
	//	CoreUtils::InputPin<1> enable;

	//	CoreUtils::OutputPin<1> io_read;
	//	CoreUtils::OutputPin<1> io_write;
	//	CoreUtils::OutputPin<8> read_device;
	//	CoreUtils::OutputPin<1> irq;

	//private:
	//	void trigger_irq();
	//	CoreUtils::Multiplexer<bit_width, 3> data_out_mux;
	//	CoreUtils::Decoder<3> enable_decoder;

	//	CoreUtils::Register<8> port_a_data;
	//	CoreUtils::Register<8> port_b_data;
	//	CoreUtils::Register<8> uart_out_data;

	//	CoreUtils::Register<1> irq_enable;
	//	CoreUtils::ANDGate<1> write_enable_and;
	//};

	//template <size_t bit_width>
	//PeripheralInterfaceAdapter<bit_width>::PeripheralInterfaceAdapter()
	//{
	//	address.on_state_change = [this](bitset<bit_width> new_data)
	//	{
	//		auto split_data = CoreUtils::ZeroExtend<bit_width, 3>::calc_zero_extend(new_data);
	//		data_out_mux.select.set_data(split_data);
	//		enable_decoder.select.set_data(split_data);
	//		
	//	};

	//	port_a_in.on_state_change = [this](bitset<8> new_data)
	//		{
	//			data_out_mux.data_in[0].set_data(CoreUtils::ZeroExtend<8, bit_width>::calc_zero_extend(new_data));
	//			trigger_irq();
	//		};
	//	port_a_data.data_out.on_state_change = [this](bitset<8> new_data)
	//		{
	//			port_a_out.set_data(new_data);
	//		};

	//	port_b_in.on_state_change = [this](bitset<8> new_data)
	//		{
	//			data_out_mux.data_in[2].set_data(CoreUtils::ZeroExtend<8, bit_width>::calc_zero_extend(new_data));
	//			trigger_irq();
	//		};
	//	port_b_data.data_out.on_state_change = [this](bitset<8> new_data)
	//		{
	//			port_b_out.set_data(new_data);
	//		};

	//	uart_in.on_state_change = [this](bitset<8> new_data)
	//		{
	//			data_out_mux.data_in[4].set_data(CoreUtils::ZeroExtend<8, bit_width>::calc_zero_extend(new_data));
	//			trigger_irq();
	//		};
	//	uart_out_data.data_out.on_state_change = [this](bitset<8> new_data)
	//		{
	//			uart_out.set_data(new_data);
	//		};

	//	data_out_mux.data_out.on_state_change = [this](bitset<bit_width> new_data)
	//		{
	//			data_out.set_data(new_data);
	//		};
	//}

	//template <size_t bit_width>
	//void PeripheralInterfaceAdapter<bit_width>::trigger_irq()
	//{
	//}
}
