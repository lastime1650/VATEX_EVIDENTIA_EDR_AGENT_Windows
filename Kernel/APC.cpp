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
				������忡�� �� �����͸� �ν��ϴ� �뵵

			* 2. [PVOID UserAllocatedData]
				Arguments 2 in APC Queue
				�ַ� ���Ḯ��Ʈ ���·� �����͸� ����.
				* �� �����ʹ� ������忡 �Ҵ�� �����ּҿ��� �Ѵ�. 
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
				NULL // ������� APC �ݹ��Լ��� ������ �� ( NornalContext )  -> NULL
			);

			if (!KeInsertQueueApc(
				Response_APC,
				(PVOID)cmd,// �״�� ���� ( Argument 1)
				UserAllocatedData, // ũ��� cmd�� ������ ����ü�� ĳ�����ϵ��� ����� ( Argument 2 )
				0)
			) {
				ExFreePoolWithTag(Response_APC, APC_ALLOC_TAG);
				return FALSE;
			}
			else {
				/* KernelApcCleanup ��ƾ�� �����Ѵ�. */
			}

			return TRUE;
		}



		extern "C" VOID NTAPI KernelApcCleanup(PKAPC Apc, PKNORMAL_ROUTINE* NormalRoutine, PVOID* NormalContext, PVOID* SystemArgument1, PVOID* SystemArgument2)
		{
			UNREFERENCED_PARAMETER(NormalRoutine);
			UNREFERENCED_PARAMETER(NormalContext);
			UNREFERENCED_PARAMETER(SystemArgument1);
			UNREFERENCED_PARAMETER(SystemArgument2);

			// KeInsertQueueApc�� ����� APC ��ü�� �����մϴ�.
			ExFreePoolWithTag(Apc, APC_ALLOC_TAG);
		}
	}
}