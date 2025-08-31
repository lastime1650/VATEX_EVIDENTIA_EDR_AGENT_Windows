#include "Timestamp.hpp"

namespace EDR
{
	namespace Util
	{
        namespace Timestamp
        {
            ULONG32 Get_Timestamp(_Inout_ PCHAR* Timestamp)
            {
                if (!Timestamp)
                    return 0;

                *Timestamp = (PCHAR)ExAllocatePool2(POOL_FLAG_NON_PAGED, output_time_buffer_size, TIMESTAMP_ALLOC_TAG);
                if (*Timestamp == NULL) return 0;

                LARGE_INTEGER systemTime, localTime;
                TIME_FIELDS timeFields;

                // 1. 현재 시스템 시간을 얻기
                KeQuerySystemTimePrecise(&systemTime);

                // 2. 시스템 시간을 로컬 시간으로 변환
                ExSystemTimeToLocalTime(&systemTime, &localTime);

                // 3. 로컬 시간을 TIME_FIELDS 구조체로 변환
                RtlTimeToTimeFields(&localTime, &timeFields);

                // 4. ANSI 형식으로 변환 (YYYY-MM-DD HH:MM:SS.mmm)
                RtlStringCbPrintfA(
                    *Timestamp,
                    output_time_buffer_size,// 길이 input ( null 포함 ) 
                    "%04hu-%02hu-%02hu %02hu:%02hu:%02hu.%03hu",
                    timeFields.Year,
                    timeFields.Month,
                    timeFields.Day,
                    timeFields.Hour,
                    timeFields.Minute,
                    timeFields.Second,
                    timeFields.Milliseconds
                );

                return output_time_buffer_size;
            }

            VOID Release_Got_Time(
                _In_ PCHAR Timestamp      // 시간 정보를 담을 버퍼
            ) {
                if (!Timestamp)
                    return;

                ExFreePoolWithTag(Timestamp, TIMESTAMP_ALLOC_TAG);
            }
        }
		
	}
}

