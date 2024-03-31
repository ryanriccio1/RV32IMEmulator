#pragma once
#include "alu.h"
#include "alu_src.h"
#include "branch.h"
#include "control_logic.h"
#include "forward_control.h"
#include "hazard_control.h"
#include "immediate_decode.h"
#include "irq.h"
#include "mem_align.h"
#include "pipeline_registers.h"
#include "program_counter.h"
#include "registers.h"

namespace RISCV
{
	using namespace std;
	template <size_t bit_width>
	class Core
	{
	public:
		Core();
		CoreUtils::InputPin<1> clock;
		CoreUtils::InputPin<1> reset;
		CoreUtils::InputPin<bit_width> data_in;
		CoreUtils::InputPin<1> irq;

		CoreUtils::OutputPin<bit_width> address;
		CoreUtils::OutputPin<bit_width> data_out;
		CoreUtils::OutputPin<2> data_type;
		CoreUtils::OutputPin<1> memory_read_en;
		CoreUtils::OutputPin<1> memory_write_en;

		void reset_all();
		//const vector<CoreUtils::Register<bit_width>>& register_reader;	// for debugging


	private:
		CoreComp::IF_ID<bit_width> if_id;
		CoreComp::ID_EX<bit_width> id_ex;
		CoreComp::EX_MEM<bit_width> ex_mem;
		CoreComp::MEM_WB<bit_width> mem_wb;

		CoreComp::HazardControl *hazard_control; 
		CoreComp::ForwardControl<bit_width> forward_control;
		CoreComp::IRQControl<bit_width> irq_control;

		CoreComp::ProgramCounter<bit_width> PC;
		CoreComp::Registers<bit_width, 5> registers;

		CoreComp::BranchLogic<bit_width> branch;
		CoreComp::ImmediateDecode<bit_width> imm_decode;
		CoreComp::ControlUnit<bit_width> control_unit;
		CoreComp::MemAlign<bit_width> mem_align;

		CoreComp::ALU<bit_width> alu;
		CoreComp::ALUSource<bit_width> alu_src_a;
		CoreComp::ALUSource<bit_width> alu_src_b;

		CoreUtils::Multiplexer<bit_width, 1> address_src_mux;
		CoreUtils::Multiplexer<bit_width, 1> if_src_mux;
		CoreUtils::Multiplexer<bit_width, 1> mem_fw_src_mux;
		CoreUtils::Multiplexer<bit_width, 2> wb_src_mux;

		CoreUtils::ANDGate<1> if_nop_and;
		CoreUtils::ANDGate<1> branch_logic_and;
		CoreUtils::ORGate<1> jump_logic_or;

		bool first_clock_cycle;
	};

	template <size_t bit_width>
	Core<bit_width>::Core() : first_clock_cycle(true), hazard_control(new CoreComp::HazardControl)
	{
		PC.PC_src.set_data(0);
		clock.on_state_change = [this](bitset<1> new_data)
		{
			registers.clock.set_data(new_data);
			hazard_control->clock.set_data(new_data);

			mem_wb.clock.set_data(new_data);
			ex_mem.clock.set_data(new_data);
			id_ex.clock.set_data(new_data);
			forward_control.clock.set_data(new_data);
			if_id.clock.set_data(new_data);

			if (!first_clock_cycle)
				PC.clock.set_data(new_data);
			else if (new_data == 0)
				first_clock_cycle = false;
			irq_control.clock.set_data(new_data);
		};
		reset.on_state_change = [this](bitset<1> new_data)
		{
			ex_mem.reset.set_data(new_data);
			mem_wb.reset.set_data(new_data);

			hazard_control->reset.set_data(new_data);
			irq_control.reset.set_data(new_data);

			PC.reset.set_data(new_data);
			registers.reset.set_data(new_data);
		};
		data_in.on_state_change = [this](bitset<bit_width> new_data)
		{
			mem_align.data_in.set_data(new_data);
			if_src_mux.data_in[0].set_data(new_data);
		};
		irq.on_state_change = [this](bitset<1> new_data)
		{
			irq_control.irq_in.set_data(new_data);
		};
		PC.PC.on_state_change = [this](bitset<bit_width> new_data)
		{
			if_id.PC.set_data(new_data);
			address_src_mux.data_in[0].set_data(new_data);
		};
		PC.PC4.on_state_change = [this](bitset<bit_width> new_data)
		{
			if_id.PC4.set_data(new_data);
		};
		irq_control.pc_stop.on_state_change = [this](bitset<1> new_data)
		{
			PC.irq.set_data(new_data);
			if_src_mux.select.set_data(new_data);
		};
		irq_control.instruction_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			if_src_mux.data_in[1].set_data(new_data);
		};
		irq_control.insert_nop.on_state_change = [this](bitset<1> new_data)
		{
			if_nop_and.a.set_data(new_data.flip());
		};
		if_src_mux.data_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			if_id.instruction.set_data(new_data);
		};
		if_nop_and.data_out.on_state_change = [this](bitset<1> new_data)
		{
			if_id.PC_en.set_data(new_data);
		};
		address_src_mux.data_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			address.set_data(new_data);
		};
		if_id.PC_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			id_ex.PC.set_data(new_data);
		};
		if_id.PC4_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			id_ex.PC4.set_data(new_data);
		};
		if_id.instruction_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			imm_decode.instruction.set_data(new_data);
			control_unit.data_in.set_data(new_data);
			id_ex.instruction.set_data(new_data);
		};
		if_id.rs1.on_state_change = [this](bitset<5> new_data)
		{
			registers.rs1.set_data(new_data);
		};
		if_id.rs2.on_state_change = [this](bitset<5> new_data)
		{
			registers.rs2.set_data(new_data);
		};
		registers.a.on_state_change = [this](bitset<bit_width> new_data)
		{
			id_ex.reg_a.set_data(new_data);
		};
		registers.b.on_state_change = [this](bitset<bit_width> new_data)
		{
			id_ex.reg_b.set_data(new_data);
		};
		imm_decode.immediate.on_state_change = [this](bitset<bit_width> new_data)
		{
			id_ex.immediate.set_data(new_data);
		};
		control_unit.data_out.on_state_change = [this](bitset<11> new_data)
		{
			id_ex.control_word.set_data(new_data);
		};
		id_ex.jump.on_state_change = [this](bitset<1> new_data)
		{
			jump_logic_or.a.set_data(new_data);
		};
		id_ex.branch.on_state_change = [this](bitset<1> new_data)
		{
			branch_logic_and.a.set_data(new_data);
		};
		id_ex.funct3.on_state_change = [this](bitset<3> new_data)
		{
			branch.funct3.set_data(new_data);
			alu.funct3.set_data(new_data);
			ex_mem.funct3.set_data(new_data);
		};
		id_ex.reg_a_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			alu_src_a.reg_data.set_data(new_data);
		};
		id_ex.PC_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			alu_src_a.opt_data.set_data(new_data);
		};
		id_ex.alu_src_1.on_state_change = [this](bitset<1> new_data)
		{
			alu_src_a.sel_reg.set_data(new_data);
		};
		id_ex.funct7.on_state_change = [this](bitset<7> new_data)
		{
			alu.funct7.set_data(new_data);
		};
		id_ex.alu_op.on_state_change = [this](bitset<2> new_data)
		{
			alu.alu_op.set_data(new_data);
		};
		id_ex.reg_b_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			alu_src_b.reg_data.set_data(new_data);
		};
		id_ex.immediate_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			alu_src_b.opt_data.set_data(new_data);
		};
		id_ex.alu_src_2.on_state_change = [this](bitset<1> new_data)
		{
			alu_src_b.sel_reg.set_data(new_data);
		};
		id_ex.reg_dest.on_state_change = [this](bitset<5> new_data)
		{
			ex_mem.reg_dest.set_data(new_data);
		};
		id_ex.PC4_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			ex_mem.PC4.set_data(new_data);
		};
		id_ex.control_word_out.on_state_change = [this](bitset<11> new_data)
		{
			ex_mem.control_word.set_data(new_data);
		};
		id_ex.rs1.on_state_change = [this](bitset<5> new_data)
		{
			forward_control.rs1.set_data(new_data);
		};
		id_ex.rs2.on_state_change = [this](bitset<5> new_data)
		{
			forward_control.rs2.set_data(new_data);
		};
		forward_control.fw_data1.on_state_change = [this](bitset<bit_width> new_data)
		{
			alu_src_a.fw_data.set_data(new_data);
		};
		forward_control.fw_en1.on_state_change = [this](bitset<1> new_data)
		{
			alu_src_a.sel_forward.set_data(new_data);
		};
		forward_control.fw_data2.on_state_change = [this](bitset<bit_width> new_data)
		{
			alu_src_b.fw_data.set_data(new_data);
		};
		forward_control.fw_en2.on_state_change = [this](bitset<1> new_data)
		{
			alu_src_b.sel_forward.set_data(new_data);
		};
		alu_src_a.to_branch.on_state_change = [this](bitset<bit_width> new_data)
		{
			branch.a.set_data(new_data);
		};
		alu_src_a.to_alu.on_state_change = [this](bitset<bit_width> new_data)
		{
			alu.a.set_data(new_data);
		};
		alu_src_b.to_branch.on_state_change = [this](bitset<bit_width> new_data)
		{
			branch.b.set_data(new_data);
			ex_mem.reg_b.set_data(new_data);
		};
		alu_src_b.to_alu.on_state_change = [this](bitset<bit_width> new_data)
		{
			alu.b.set_data(new_data);
		};
		alu.result.on_state_change = [this](bitset<bit_width> new_data)
		{
			PC.PC_in.set_data(new_data);
			ex_mem.alu_result.set_data(new_data);
		};
		branch.branch.on_state_change = [this](bitset<1> new_data)
		{
			branch_logic_and.b.set_data(new_data);
		};
		branch_logic_and.data_out.on_state_change = [this](bitset<1> new_data)
		{
			jump_logic_or.b.set_data(new_data);
		};
		jump_logic_or.data_out.on_state_change = [this](bitset<1> new_data)
		{
			ex_mem.jump_branch.set_data(new_data);
			hazard_control->jump_branch_ex.set_data(new_data);
			PC.PC_src.set_data(new_data);
		};
		hazard_control->flush.on_state_change = [this](bitset<1> new_data)
		{
			id_ex.reset.set_data(new_data);
			if_id.reset.set_data(new_data);
		};
		hazard_control->PC_en.on_state_change = [this](bitset<1> new_data)
		{
			PC.PC_en.set_data(new_data);
			if_nop_and.b.set_data(new_data);
		};
		hazard_control->address_source.on_state_change = [this](bitset<1> new_data)
		{
			address_src_mux.select.set_data(new_data);
		};
		ex_mem.funct3_out.on_state_change = [this](bitset<3> new_data)
		{
			data_type.set_data(CoreUtils::ZeroExtend<3, 2>::calc_zero_extend(new_data));
			mem_align.funct3.set_data(new_data);
		};
		ex_mem.reg_b_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			data_out.set_data(new_data);
		};
		ex_mem.alu_result_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			mem_fw_src_mux.data_in[1].set_data(new_data);
			mem_wb.alu_result.set_data(new_data);
			address_src_mux.data_in[1].set_data(new_data);
		};
		ex_mem.mem_read.on_state_change = [this](bitset<1> new_data)
		{
			memory_read_en.set_data(new_data);
			hazard_control->mem_read.set_data(new_data);
			mem_fw_src_mux.select.set_data(new_data.flip());
		};
		ex_mem.mem_write.on_state_change = [this](bitset<1> new_data)
		{
			memory_write_en.set_data(new_data);
			hazard_control->mem_write.set_data(new_data);
		};
		ex_mem.reg_dest_out.on_state_change = [this](bitset<5> new_data)
		{
			mem_wb.reg_dest.set_data(new_data);
			forward_control.mem_reg_dest.set_data(new_data);
		};
		ex_mem.PC4_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			mem_wb.PC4.set_data(new_data);
		};
		ex_mem.control_word_out.on_state_change = [this](bitset<11> new_data)
		{
			mem_wb.control_word.set_data(new_data);
		};
		ex_mem.rf_we.on_state_change = [this](bitset<1> new_data)
		{
			forward_control.mem_reg_write.set_data(new_data);
		};
		ex_mem.jump_branch_out.on_state_change = [this](bitset<1> new_data)
		{
			hazard_control->jump_branch_mem.set_data(new_data);
		};
		mem_fw_src_mux.data_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			forward_control.data_mem.set_data(new_data);
		};
		mem_align.data_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			mem_wb.memory.set_data(new_data);
			mem_fw_src_mux.data_in[0].set_data(new_data);
		};
		mem_wb.memory_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			wb_src_mux.data_in[0].set_data(new_data);
		};
		mem_wb.PC4_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			wb_src_mux.data_in[1].set_data(new_data);
		};
		mem_wb.alu_result_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			wb_src_mux.data_in[2].set_data(new_data);
		};
		mem_wb.rf_src.on_state_change = [this](bitset<2> new_data)
		{
			wb_src_mux.select.set_data(new_data);
		};
		mem_wb.reg_dest_out.on_state_change = [this](bitset<5> new_data)
		{
			registers.rd.set_data(new_data);
			forward_control.wb_reg_dest.set_data(new_data);
		};
		mem_wb.rf_we.on_state_change = [this](bitset<1> new_data)
		{
			registers.we.set_data(new_data);
			forward_control.wb_reg_write.set_data(new_data);
		};
		wb_src_mux.data_out.on_state_change = [this](bitset<bit_width> new_data)
		{
			registers.data_in.set_data(new_data);
			forward_control.data_wb.set_data(new_data);
		};
	}

	template <size_t bit_width>
	void Core<bit_width>::reset_all()
	{
		reset.set_data(1);
		reset.set_data(0);
	}

}
