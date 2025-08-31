#include "IOCTL_Routines.hpp"
#include "APC.hpp"

HANDLE User_PID = NULL;
HANDLE User_HANDLE = NULL;

#include "APC.hpp"

namespace EDR
{
	namespace Util
	{
		namespace IOCTL
		{
			

			// 초기화
			namespace Initialize
			{

				BOOLEAN Initialize(IOCTL_STRUCT::PIOCTL_INIT_STRUCT parameter)
				{
					// 드라이버 초기화 작업

					// 0. IOCTL 유저 PID얻기 ( 강제종료 보호 및 APC 전달 타겟 및 가상주소할당 용 )
					User_PID = parameter->IOCTL_Request_User_PID;
					if (!NT_SUCCESS(EDR::Util::Process::Handle::LookupProcessHandlebyProcessId(User_PID, &User_HANDLE)))
						return FALSE;

					__debugbreak();
					// 1. APC 등록
					if (!EDR::APC::INITIALIZE_APC(
						parameter->APC_INFO.ThreadID,
						parameter->APC_INFO.User_APCHandler
					))
					{
						return FALSE;
					}

					return TRUE;
				}

			}
		}
	}
}