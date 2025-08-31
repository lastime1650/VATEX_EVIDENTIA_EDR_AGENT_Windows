#ifndef IOCTL_INIT_H
#define IOCTL_INIT_H

#include <ntifs.h>

#include "util.hpp"


#define IOCTL_INIT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1650, METHOD_BUFFERED, FILE_ANY_ACCESS) // �ʱ�ȭ ���

extern HANDLE User_PID;
extern HANDLE User_HANDLE;


namespace EDR
{
	namespace Util
	{
		namespace IOCTL
		{
			// ������尣 �ʱ�ȭ �۾�
			namespace Initialize
			{
				namespace IOCTL_STRUCT
				{
					typedef struct IOCTL_INIT_STRUCT_
					{
						HANDLE IOCTL_Request_User_PID;
						
						// APC �ʿ� �ʱ�ȭ ����
						struct
						{
							HANDLE ThreadID;
							PVOID User_APCHandler;
						}APC_INFO;

						BOOLEAN output;

					}IOCTL_INIT_STRUCT, *PIOCTL_INIT_STRUCT;
					static_assert(sizeof(IOCTL_INIT_STRUCT) == 32, "IOCTL_INIT_STRUCT ���̰� �ٸ�");
				}

				

				BOOLEAN Initialize(IOCTL_STRUCT::PIOCTL_INIT_STRUCT parameter);
			}

			// ��������� �ΰ��� ��û
			namespace Request
			{

			}

			// ��������� "����" ��û
			namespace Response
			{

			}

		}
	}
}

#endif