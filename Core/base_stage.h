#pragma once

#include "core.h"

namespace RV32IM
{
	class Core;
	namespace Stage
	{
		class BaseStage
		{
		public:
			virtual ~BaseStage() = default;
			BaseStage(Core* core) : core(core) {}
			virtual void clock() = 0;
			virtual void run() = 0;
		protected:
			Core* core;
		};
	}
}
