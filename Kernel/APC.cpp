#include "APC.hpp"

namespace EDR
{
	namespace APC
	{

		PETHREAD USER_Thread = NULL;
		PKNORMAL_ROUTINE USER_APC_HANDLER = NULL;

		
		BOOLEAN INITIALIZE_APC(HANDLE USER_ThreadID, PVOID USER_APC_Handler)
		{
			if (!NT_SUCCESS(PsLookupThreadByThreadId(USER_ThreadID, &USER_Thread)))
				return FALSE;

			USER_APC_HANDLER = (PKNORMAL_ROUTINE)USER_APC_Handler;

			return TRUE;
		}

		// APC to User
		BOOLEAN ApcToUser(ULONG64 cmd, PVOID UserAllocatedData)
		{
			/*
			* 
			* @parms
			* 1. [ULONG64 cmd] 
				Arguments 1 in APC Queue
				유저모드에서 이 데이터를 인식하는 용도

			* 2. [PVOID UserAllocatedData]
				Arguments 2 in APC Queue
				주로 연결리스트 형태로 데이터를 전달.
				* 이 데이터는 유저모드에 할당된 가상주소여야 한다. 
			* 
			*/
			if (!USER_Thread || !USER_APC_HANDLER)
				return FALSE;

			PKAPC Response_APC = (PKAPC)ExAllocatePoolWithTag(NonPagedPool, sizeof(KAPC), APC_ALLOC_TAG);
			if (!Response_APC)
			{
				return FALSE;
			}

			KeInitializeApc(
				Response_APC,
				USER_Thread,
				OriginalApcEnvironment,
				KernelApcCleanup,
				NULL,
				(PKNORMAL_ROUTINE)USER_APC_HANDLER, // USER ADDRESS CALLBACK ! 
				UserMode,
				NULL // 유저모드 APC 콜백함수에 전달할 것 ( NornalContext )  -> NULL
			);

			if (!KeInsertQueueApc(
				Response_APC,
				(PVOID)cmd,// 그대로 삽입 ( Argument 1)
				UserAllocatedData, // 크기는 cmd에 정해진 구조체로 캐스팅하도록 설계됨 ( Argument 2 )
				0)
			) {
				ExFreePoolWithTag(Response_APC, APC_ALLOC_TAG);
				return FALSE;
			}
			else {
				/* KernelApcCleanup 루틴이 해제한다. */
			}

			return TRUE;
		}



		extern "C" VOID NTAPI KernelApcCleanup(PKAPC Apc, PKNORMAL_ROUTINE* NormalRoutine, PVOID* NormalContext, PVOID* SystemArgument1, PVOID* SystemArgument2)
		{
			UNREFERENCED_PARAMETER(NormalRoutine);
			UNREFERENCED_PARAMETER(NormalContext);
			UNREFERENCED_PARAMETER(SystemArgument1);
			UNREFERENCED_PARAMETER(SystemArgument2);

			// KeInsertQueueApc가 사용한 APC 객체를 해제합니다.
			ExFreePoolWithTag(Apc, APC_ALLOC_TAG);
		}
	}
}