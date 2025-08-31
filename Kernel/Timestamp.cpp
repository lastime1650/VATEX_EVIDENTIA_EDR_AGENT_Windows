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

                // 1. ���� �ý��� �ð��� ���
                KeQuerySystemTimePrecise(&systemTime);

                // 2. �ý��� �ð��� ���� �ð����� ��ȯ
                ExSystemTimeToLocalTime(&systemTime, &localTime);

                // 3. ���� �ð��� TIME_FIELDS ����ü�� ��ȯ
                RtlTimeToTimeFields(&localTime, &timeFields);

                // 4. ANSI �������� ��ȯ (YYYY-MM-DD HH:MM:SS.mmm)
                RtlStringCbPrintfA(
                    *Timestamp,
                    output_time_buffer_size,// ���� input ( null ���� ) 
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
                _In_ PCHAR Timestamp      // �ð� ������ ���� ����
            ) {
                if (!Timestamp)
                    return;

                ExFreePoolWithTag(Timestamp, TIMESTAMP_ALLOC_TAG);
            }
        }
		
	}
}

