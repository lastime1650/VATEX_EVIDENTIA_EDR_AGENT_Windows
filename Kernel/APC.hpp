#ifndef APC_H
#define APC_H

#include "util.hpp"

namespace EDR
{
	namespace APC
	{

		#define APC_ALLOC_TAG 'APCu'

		extern PETHREAD USER_Thread;
		extern PKNORMAL_ROUTINE USER_APC_HANDLER; // IOCTL ������ �����ּ��� APC �Լ��ּ�

		BOOLEAN INITIALIZE_APC(HANDLE USER_ThreadID, PVOID USER_APC_Handler); // APC �������� �ʱ�ȭ

		BOOLEAN ApcToUser(ULONG64 cmd, PVOID UserAllocatedData);// APC ť��� ������忡�� �񵿱��� ������ ����


		extern "C" VOID NTAPI KernelApcCleanup(PKAPC Apc, PKNORMAL_ROUTINE* NormalRoutine, PVOID* NormalContext, PVOID* SystemArgument1, PVOID* SystemArgument2);
	}
}


#endif