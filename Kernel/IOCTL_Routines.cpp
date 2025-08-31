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
			

			// �ʱ�ȭ
			namespace Initialize
			{

				BOOLEAN Initialize(IOCTL_STRUCT::PIOCTL_INIT_STRUCT parameter)
				{
					// ����̹� �ʱ�ȭ �۾�

					// 0. IOCTL ���� PID��� ( �������� ��ȣ �� APC ���� Ÿ�� �� �����ּ��Ҵ� �� )
					User_PID = parameter->IOCTL_Request_User_PID;
					if (!NT_SUCCESS(EDR::Util::Process::Handle::LookupProcessHandlebyProcessId(User_PID, &User_HANDLE)))
						return FALSE;

					__debugbreak();
					// 1. APC ���
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