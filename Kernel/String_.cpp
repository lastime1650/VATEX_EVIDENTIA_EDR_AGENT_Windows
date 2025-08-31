#include "String_.hpp"

namespace EDR
{
	namespace Util
	{
		namespace String
		{
			BOOLEAN UnicodeString_to_ANSI(PCUNICODE_STRING Input_Unicode, PCHAR* AnsiString, ULONG32* AnsiStringMaxLength)
			{
				if (!Input_Unicode || !AnsiString || !AnsiStringMaxLength)
					return FALSE;

				ANSI_STRING ansi;
				if (!NT_SUCCESS(RtlUnicodeStringToAnsiString(&ansi, (PCUNICODE_STRING)Input_Unicode, TRUE)))
				{
					return FALSE;
				}

				*AnsiString = (PCHAR)ExAllocatePool2(POOL_FLAG_NON_PAGED, ansi.MaximumLength, UnicodeString_to_CHAR_ALLOC_TAG);
				if (!*AnsiString)
					return FALSE;
				RtlCopyMemory(*AnsiString, ansi.Buffer, ansi.MaximumLength);

				*AnsiStringMaxLength = ansi.MaximumLength;


				RtlFreeAnsiString(&ansi);
				return TRUE;
			}
			VOID Release_UnicodeString_to_ANSI(PCHAR allocated_string)
			{
				if (allocated_string)
					ExFreePoolWithTag(allocated_string, UnicodeString_to_CHAR_ALLOC_TAG);
			}
		}
	}
	
}