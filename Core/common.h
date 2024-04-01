#pragma once
#include <cstdint>

namespace RV32IM
{
	using namespace std;

	using unsigned_data = uint32_t;
	using signed_data = int32_t;
	using inst_data = uint32_t;

	enum Opcodes
	{
		LUI = 0b0110111,
		AUIPC = 0b0010111,
		JAL = 0b1101111,
		JALR = 0b1100111,
		BXX = 0b1100011,
		LX = 0b0000011,
		SX = 0b0100011,
		RI = 0b0010011,
		RR = 0b0110011
	};

	enum Funct3
	{
		ADD		= 0x0,
		SUB		= 0x0,
		SLL		= 0x1,
		SLT		= 0x2,
		SLTU	= 0x3,
		XOR		= 0x4,
		SRL		= 0x5,
		SRA		= 0x5,
		OR		= 0x6,
		AND		= 0x7,
		MUL		= 0x0,
		MULH	= 0x1,
		MULHSU	= 0x2,
		MULHU	= 0x3,
		DIV		= 0x4,
		REM		= 0x5,
		DIVU	= 0x6,
		REMU	= 0x7,
		LB		= 0x0,
		LH		= 0x1,
		LW		= 0x2,
		LBU		= 0x4,
		LHU		= 0x5,
		SB		= 0x0,
		SH		= 0x1,
		SW		= 0x2,
		BEQ		= 0x0,
		BNE		= 0x1,
		BLT		= 0x4,
		BGE		= 0x5,
		BLTU	= 0x6,
		BGEU	= 0x7
	};

	enum Funct7
	{
		NORM	= 0x00,
		INV		= 0x20,
		M_EXT	= 0x01
	};

	enum class InstructionFormat { R, I, S, B, U, J };

	constexpr unsigned_data generate_bitmask(size_t bit_width);
	unsigned_data mask_data(unsigned_data data, size_t low_bit, size_t high_bit);
	unsigned_data sign_extend(unsigned_data data, size_t space);
}
