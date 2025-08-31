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
					Input,           // 입력 버퍼로 scanParams 전달
					sizeof(IOCTL_STRUCT::IOCTL_INIT_STRUCT),    // 입력 버퍼 크기
					Output,           // 출력 버퍼로도 동일한 scanParams 전달
					sizeof(IOCTL_STRUCT::IOCTL_INIT_STRUCT),    // 출력 버퍼 크기
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