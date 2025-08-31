#include "IOCTL.hpp"

namespace EDR
{
	namespace IOCTL
	{
		HANDLE IOCTL_HANDLE = NULL;

		BOOLEAN ConnectIOCTL()
		{
			IOCTL_HANDLE = CreateFileW(
				IOCTL_Device_SymbolicName,
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				0,
				NULL
			);
			if (IOCTL_HANDLE == INVALID_HANDLE_VALUE)
				return FALSE;

			return TRUE;
		}

		VOID DisconnectIOCTL()
		{
			if (!IOCTL_HANDLE || IOCTL_HANDLE == INVALID_HANDLE_VALUE)
				return;
			CloseHandle(IOCTL_HANDLE);
		}

		namespace Initialize
		{
			BOOLEAN Initialize(IOCTL_STRUCT::PIOCTL_INIT_STRUCT Input)
			{

				IOCTL_STRUCT::PIOCTL_INIT_STRUCT Output = Input;

				DWORD bytesReturned = 0;
				BOOL result = DeviceIoControl(
					IOCTL_HANDLE,
					IOCTL_INIT,
					Input,           // �Է� ���۷� scanParams ����
					sizeof(IOCTL_STRUCT::IOCTL_INIT_STRUCT),    // �Է� ���� ũ��
					Output,           // ��� ���۷ε� ������ scanParams ����
					sizeof(IOCTL_STRUCT::IOCTL_INIT_STRUCT),    // ��� ���� ũ��
					&bytesReturned,
					NULL
				);

				if (!result) {
					CloseHandle(IOCTL_HANDLE);
					return FALSE;
				}

				return Output->output;

			}
		}

	}
}