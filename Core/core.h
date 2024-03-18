#pragma once
#include "alu.h"
#include "alu_src.h"
#include "branch.h"
#include "control_logic.h"
#include "forward_control.h"
#include "hazard_control.h"
#include "immediate_decode.h"
#include "mem_align.h"
#include "pipeline_registers.h"
#include "program_counter.h"
#include "registers.h"

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
		CoreComp::ImmediateDecode<32> imm_decode;
		CoreComp::ControlUnit<32> control_unit;
		CoreComp::MemAlign<32> mem_align;
		CoreComp::ALUSource<32> alu_src_a;
		CoreComp::BranchLogic<32> branch;
		CoreComp::HazardControl hazard;
		CoreComp::ForwardControl<32> forward_control;
		CoreComp::ALU<32> alu;

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
