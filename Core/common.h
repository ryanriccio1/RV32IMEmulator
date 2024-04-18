#pragma once
#include <cstdint>
#include <string>
#include <map>

namespace RV32IM
{
	using namespace std;

	using unsigned_data = uint32_t;
	using signed_data = int32_t;
	using inst_data = uint32_t;

	static constexpr unsigned_data vga_mode = 0xFFF00;
	static constexpr unsigned_data vga_mem_ptr = 0xFFF04;
	static constexpr unsigned_data col_mem_ptr = 0xFFF08;
	static constexpr unsigned_data chr_mem_ptr = 0xFFF0C;

	static constexpr unsigned_data irq_mask = 0xFFFF0;
	static constexpr unsigned_data timer_in = 0xFFFF1;
	static constexpr unsigned_data keyboard_in = 0xFFFF2;
	static constexpr unsigned_data uart_tx = 0xFFFF3;
	static constexpr unsigned_data uart_rx = 0xFFFF4;
	static constexpr unsigned_data irq_en = 0xFFFF5;
	static constexpr unsigned_data irq_handle = 0xFFFF6;
	static constexpr unsigned_data irq_vector = 0xFFFF7;

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

	enum RegisterName
	{
		zero, ra, sp, gp, tp, t0, t1, t2, s0, s1, a0, a1, a2, a3, a4, a5,
		a6, a7, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, t3, t4, t5, t6
	};

	inline map<RegisterName, string> register_name_to_string = 
	{
		{zero, "zero"},
		{ra, "ra"},
		{sp, "sp"},
		{gp, "gp"},
		{tp, "tp"},
		{t0, "t0"},
		{t1, "t1"},
		{t2, "t2"},
		{s0, "s0"},
		{s1, "s1"},
		{a0, "a0"},
		{a1, "a1"},
		{a2, "a2"},
		{a3, "a3"},
		{a4, "a4"},
		{a5, "a5"},
		{a6, "a6"},
		{a7, "a7"},
		{s2, "s2"},
		{s3, "s3"},
		{s4, "s4"},
		{s5, "s5"},
		{s6, "s6"},
		{s7, "s7"},
		{s8, "s8"},
		{s9, "s9"},
		{s10, "s10"},
		{s11, "s11"},
		{t3, "t3"},
		{t4, "t4"},
		{t5, "t5"},
		{t6, "t6"},
	};

	enum IRQType
	{
		KEYBOARD = 1, UART_RX = 2, TIMER = 4
	};

	enum VideoModes
	{
		BITMAP, CHARACTER
	};

	enum class InstructionFormat { R, I, S, B, U, J };

	constexpr unsigned_data generate_bitmask(size_t bit_width);
	unsigned_data mask_data(unsigned_data data, size_t low_bit, size_t high_bit);
	unsigned_data sign_extend(unsigned_data data, size_t space);
}
