#pragma once
#include "utils.h"

namespace RISCV
{
	namespace CoreComp
	{
		using namespace std;

		template <size_t bit_width>
		class ControlUnit
		{
		public:
			ControlUnit();
			CoreUtils::InputPin<bit_width> data_in;
			CoreUtils::OutputPin<11> data_out;
		};

		template <size_t bit_width>
		ControlUnit<bit_width>::ControlUnit()
		{
			enum Opcode
			{
				JP =			0b00000000001,
				BR =			0b00000000010,
				RF_SRC_MEM =	0b00000000000,
				RF_SRC_PC4 =	0b00000000100,
				RF_SRC_ALU =	0b00000001000,
				RF_WE =			0b00000010000,
				ALU_OP_ADD =	0b00000000000,
				ALU_OP_RTYPE =	0b00000100000,
				ALU_OP_ITYPE =	0b00001000000,
				ALU_OP_BYPASS = 0b00001100000,
				ALU_A_REG =		0b00000000000,
				ALU_A_PC =		0b00010000000,
				ALU_B_REG =		0b00000000000,
				ALU_B_IMM =		0b00100000000,
				MEM_RD =		0b01000000000,
				MEM_WR =		0b10000000000

			};
			data_in.on_state_change = [this](bitset<bit_width> new_data)
			{
				auto opcode = CoreUtils::ZeroExtend<bit_width, 7>::calc_zero_extend(new_data).to_ullong();
				uint16_t control_word;
				switch (opcode)
				{
				case 0b0110111:	// LUI
					control_word = RF_SRC_ALU | RF_WE | ALU_OP_BYPASS | ALU_A_REG | ALU_B_IMM;
					break;
				case 0b0010111:	// AUIPC
					control_word = RF_SRC_ALU | RF_WE | ALU_OP_ADD | ALU_A_PC | ALU_B_IMM;
					break;
				case 0b1101111:	// JAL
					control_word = JP | RF_SRC_PC4 | RF_WE | ALU_OP_ADD | ALU_A_PC | ALU_B_IMM;
					break;
				case 0b1100111:	// JALR
					control_word = JP | RF_SRC_PC4 | RF_WE | ALU_OP_ADD | ALU_A_REG | ALU_B_IMM;
					break;
				case 0b1100011:	// BXX
					control_word = BR | RF_SRC_MEM | ALU_OP_ADD | ALU_A_PC | ALU_B_IMM;
					break;
				case 0b0000011:	// LX
					control_word = RF_SRC_MEM | RF_WE | ALU_OP_ADD | ALU_A_REG | ALU_B_IMM | MEM_RD;
					break;
				case 0b0100011:	// SX
					control_word = RF_SRC_MEM | ALU_OP_ADD | ALU_A_REG | ALU_B_IMM | MEM_WR;
					break;
				case 0b0010011:	// R-IMM
					control_word = RF_SRC_ALU | RF_WE | ALU_OP_ITYPE | ALU_A_REG | ALU_B_IMM;
					break;
				case 0b0110011:	// R-Type
					control_word = RF_SRC_ALU | RF_WE | ALU_OP_RTYPE | ALU_A_REG | ALU_B_REG;
					break;
				default:
					control_word = 0x7FF;
					break;
				}
				data_out.set_data(control_word);
			};
				
		}
	}
}