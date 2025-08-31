#ifndef APC_H
#define APC_H

#include "util.hpp"

namespace EDR
{
	namespace APC
	{

		#define APC_ALLOC_TAG 'APCu'

		extern PETHREAD USER_Thread;
		extern PKNORMAL_ROUTINE USER_APC_HANDLER; // IOCTL 유저의 가상주소인 APC 함수주소

		BOOLEAN INITIALIZE_APC(HANDLE USER_ThreadID, PVOID USER_APC_Handler); // APC 전역정보 초기화

		BOOLEAN ApcToUser(ULONG64 cmd, PVOID UserAllocatedData);// APC 큐기반 유저모드에게 비동기적 데이터 전송


		extern "C" VOID NTAPI KernelApcCleanup(PKAPC Apc, PKNORMAL_ROUTINE* NormalRoutine, PVOID* NormalContext, PVOID* SystemArgument1, PVOID* SystemArgument2);
	}
}


#endif