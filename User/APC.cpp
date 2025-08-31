#include "APC.hpp"

namespace EDR
{
	namespace APC
	{
		HANDLE ThreadHandle = 0;

		extern "C" VOID NTAPI ApcHandler(PVOID NormalContext, PVOID SystemArgument1, PVOID SystemArgument2)
		{
			printf("[APC-HANDLER} NormalContext-> %p , SystemArgument1 -> %p , SystemArgument2 -> %p \n", NormalContext, SystemArgument1, SystemArgument2);
			/*
				신호에따라
				ThreadHandle 스레드 강제 종료가능
			*/
		}

		HANDLE Init_APC(HANDLE* ThreadID, PVOID* User_APCHandler)
		{
			DWORD ThreadId = 0;
			ThreadHandle = CreateThread(
				NULL,
				0,
				APC_LOOP,
				NULL,
				0,
				&ThreadId
			);

			if (ThreadHandle == INVALID_HANDLE_VALUE)
			{
				return NULL;
			}

			*ThreadID = (HANDLE)ThreadId;
			*User_APCHandler = (PVOID)ApcHandler;

			std::cout << "ThreadID: " << ThreadId << std::endl;
			std::cout << "APCHandler:" << (PVOID)ApcHandler << std::endl;

			return ThreadHandle;

		}


		extern "C" DWORD APC_LOOP(PVOID context)
		{
			while (1)
			{
				WaitForSingleObjectEx(GetCurrentThread(), INFINITE, TRUE);
			}
		}
	}
}