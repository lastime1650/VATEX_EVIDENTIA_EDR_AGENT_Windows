#ifndef UTIL_H
#define UTIL_H

#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>
#pragma warning (disable : 4201) 
#pragma warning (disable : 4083) 
#pragma warning (disable : 4005) 
#pragma warning (disable : 4100)
#pragma warning (disable : 5040)
#pragma warning (disable : 4083)
#pragma warning (disable : 4996)
#pragma warning (disable : 4189)
#include <intrin.h>

#define debug_log(text, ...) (DbgPrintEx(0, 0, /*xs*/(text), ##__VA_ARGS__))

#define debug_break() __debugbreak();

#include "API.hpp"
#include "Timestamp.hpp"
#include "String_.hpp"
#include "LinkedList.hpp"
#include "IOCTL.hpp"
#include "IOCTL_Routines.hpp"
#include "ProcessUtil.hpp"
#include "User.hpp"
#include "FileJob.hpp"
#include "HASH.hpp"
/*
	Extra
*/

namespace EDR
{
	namespace Util
	{
		VOID Delay(ULONG64 sec); // 초단위 대기

	}
}

#endif