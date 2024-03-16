#pragma once
#include "pipeline_registers.h"
#include "program_counter.h"
#include "registers.h"
#include "utils.h"

namespace RISCV
{
	using namespace std;
	class Core
	{
	public:
		Core();
		CoreComp::ProgramCounter<32> PC;
		CoreComp::IF_ID<32> if_id;
		CoreComp::ID_EX<32> id_ex;
		CoreComp::EX_MEM<32> ex_mem;
		CoreComp::MEM_WB<32> mem_wb;
		CoreComp::Registers<32, 5> registers;

		/*CoreUtils::InputPin<1> clock;
		CoreUtils::InputPin<1> reset;
		CoreUtils::InputPin<32> data_in;
		CoreUtils::InputPin<1> irq;

		CoreUtils::OutputPin<32> address;
		CoreUtils::OutputPin<32> data_out;
		CoreUtils::OutputPin<2> data_type;
		CoreUtils::OutputPin<1> memory_read_en;
		CoreUtils::OutputPin<1> memory_write_en;*/

		

	private:

	};

	Core::Core()
	{
		/*address.on_state_change = [this](uint64_t new_data)
		{
			this->data_in.set_data(new_data);
			this->irq.set_data(new_data);
		};*/
	}
}
