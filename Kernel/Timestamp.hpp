#ifndef GET_TIME_H
#define GET_TIME_H

#include <ntifs.h>
#include <ntstrsafe.h> // for RtlStringCbPrintfW

namespace EDR
{
	namespace Util
	{
		namespace Timestamp
		{
			#define TIMESTAMP_ALLOC_TAG 'Timr'
			#define output_time_buffer_size 24
			/*
				ULONG32 Get_Timestamp( _Inout_ PCHAR* Timestamp )

				��ȯ��: 1�̻��� ���̸� Ÿ�ӽ����� ����

				����:
					Timestamp: �����Ҵ�� Ÿ�ӽ����� ������
			*/
			ULONG32 Get_Timestamp(_Inout_ PCHAR* Timestamp);

			VOID Release_Got_Time(
				_In_ PCHAR Timestamp
			);
		}
	}
}



#endif
