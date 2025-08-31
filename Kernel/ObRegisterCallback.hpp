#ifndef OB_REGISETERS_CALLBACK_H
#define OB_REGISETERS_CALLBACK_H

#include "util.hpp"

namespace EDR
{
	namespace ObRegisterCallbacks
	{
		NTSTATUS Set_ObRegisterCallbacks();
		NTSTATUS UnSet_ObRegisterCallbacks();
	}
}

#endif