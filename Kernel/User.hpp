#ifndef USER_H
#define USER_H

#include <ntifs.h>

namespace EDR
{
	namespace Util
	{
		namespace User
		{
			namespace Memory
			{
				NTSTATUS AllocateMemory(HANDLE ProcessHandle, PVOID* BaseAddress, SIZE_T* DataSize);
			}
		}
	}
}

#endif