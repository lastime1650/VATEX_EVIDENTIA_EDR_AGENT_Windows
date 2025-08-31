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

				반환값: 1이상의 값이며 타임스탬프 길이

				인자:
					Timestamp: 동적할당될 타임스탬프 데이터
			*/
			ULONG32 Get_Timestamp(_Inout_ PCHAR* Timestamp);

			VOID Release_Got_Time(
				_In_ PCHAR Timestamp
			);
		}
	}
}



#endif
