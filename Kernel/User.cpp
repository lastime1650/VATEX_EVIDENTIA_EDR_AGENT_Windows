#include "User.hpp"

namespace EDR
{
	namespace Util
	{
		namespace User
		{
			namespace Memory
			{
				NTSTATUS AllocateMemory(HANDLE ProcessHandle, PVOID* BaseAddress, SIZE_T* DataSize)
				{
					if (!ProcessHandle || !BaseAddress || !DataSize || !*DataSize)
						return STATUS_INVALID_PARAMETER;

					PVOID tmp_BaseAddress = NULL;
					SIZE_T tmp_DataSize = *DataSize;

					NTSTATUS status = ZwAllocateVirtualMemory(
						ProcessHandle,
						&tmp_BaseAddress,
						0,
						&tmp_DataSize,
						MEM_COMMIT,
						PAGE_READWRITE
					);

					if(!NT_SUCCESS(status))
					{
						*BaseAddress = NULL;
					}
					else
					{
						*BaseAddress = tmp_BaseAddress;
						*DataSize = tmp_DataSize;

					}

					return status;
				}
			}
		}
	}
}