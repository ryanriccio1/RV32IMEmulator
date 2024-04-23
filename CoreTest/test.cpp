#include "pch.h"
#include "../Core/core.h"

auto core = RV32IM::Core();

TEST(Core, toggle_clock) {
	core.start_clock();
	EXPECT_TRUE(core.is_clock_running());
	core.stop_clock();
	EXPECT_FALSE(core.is_clock_running());
}
