#ifndef IOCTL_INIT_H
#define IOCTL_INIT_H

#include <ntifs.h>

#include "util.hpp"


#define IOCTL_INIT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1650, METHOD_BUFFERED, FILE_ANY_ACCESS) // 초기화 통신

extern HANDLE User_PID;
extern HANDLE User_HANDLE;


namespace EDR
{
	namespace Util
	{
		namespace IOCTL
		{
			// 유저모드간 초기화 작업
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

					}IOCTL_INIT_STRUCT, *PIOCTL_INIT_STRUCT;
					static_assert(sizeof(IOCTL_INIT_STRUCT) == 32, "IOCTL_INIT_STRUCT 길이가 다름");
				}

				

				BOOLEAN Initialize(IOCTL_STRUCT::PIOCTL_INIT_STRUCT parameter);
			}

			// 유저모드의 부가적 요청
			namespace Request
			{

			}

			// 유저모드의 "차단" 요청
			namespace Response
			{

			}

		}
	}
}

#endif