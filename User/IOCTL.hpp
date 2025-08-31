#ifndef IOCTL_H
#define IOCTL_H

#include <Windows.h>

#define IOCTL_Device_SymbolicName L"\\??\\VATEX_EVIDENTIA_EDR_AGENT"

#define IOCTL_INIT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1650, METHOD_BUFFERED, FILE_ANY_ACCESS) // 초기화 통신

namespace EDR
{
	namespace IOCTL
	{

		extern HANDLE IOCTL_HANDLE;

		BOOLEAN ConnectIOCTL();
		VOID DisconnectIOCTL();

		namespace Initialize
		{

			namespace IOCTL_STRUCT
			{
				typedef struct IOCTL_INIT_STRUCT_
				{
					HANDLE IOCTL_Request_User_PID;

					// APC 필요 초기화 정보
					struct
					{
						HANDLE ThreadID;
						PVOID User_APCHandler;
					}APC_INFO;

					BOOLEAN output;

				}IOCTL_INIT_STRUCT, * PIOCTL_INIT_STRUCT;
				static_assert(sizeof(IOCTL_INIT_STRUCT) == 32, "IOCTL_INIT_STRUCT 길이가 다름");
			}

			BOOLEAN Initialize(IOCTL_STRUCT::PIOCTL_INIT_STRUCT Input);
		}
		

	}
}

#endif