#pragma warning(disable:4312)
#include <iostream>

#include "IOCTL.hpp"
#include "APC.hpp"

int main()
{

	// IOCTL ����
	if (!EDR::IOCTL::ConnectIOCTL())
		return -1;

	// APC �ʱ�ȭ
	HANDLE ThreadID = 0;
	PVOID User_APCHandler = NULL;
	HANDLE APC_LOOP_THREAD_HANDLE = EDR::APC::Init_APC(&ThreadID, &User_APCHandler);
	if (!APC_LOOP_THREAD_HANDLE)
	{
		EDR::IOCTL::DisconnectIOCTL();
		return -1;
	}

	// IOCTL �ʱ�ȭ
	EDR::IOCTL::Initialize::IOCTL_STRUCT::IOCTL_INIT_STRUCT ioctl_init;
	ioctl_init.IOCTL_Request_User_PID = (HANDLE)GetCurrentProcessId();
	ioctl_init.APC_INFO.ThreadID = ThreadID;
	ioctl_init.APC_INFO.User_APCHandler = User_APCHandler;
	ioctl_init.output = FALSE;

	EDR::IOCTL::Initialize::Initialize(&ioctl_init);

	// EDR TCP ����



	// APC ���� ���� ���
	WaitForSingleObject(APC_LOOP_THREAD_HANDLE, INFINITE);

	return 0;
}