#ifndef HASH_H

#define HASH_H

#include <ntifs.h>
#include <ntstrsafe.h>
#include <bcrypt.h>

#define SHA256_BINARY_LENGTH      32 // SHA256 �ؽ��� ���̳ʸ� ���� (����Ʈ)
#define SHA256_STRING_LENGTH      64 // SHA256 �ؽ��� 16���� ���ڿ� ���� (NULL ����)
#define SHA256_String_Byte_Length (SHA256_STRING_LENGTH + 1) // NULL ���� ���� ����
#define HASH_ALLOC_TAG            'HahS' // �޸� �Ҵ� �±�

namespace EDR
{
	namespace Util
	{
		namespace Hash
		{
			namespace SHA256
			{
				ULONG SHA256_Hasing(PCHAR* Output_Hashed, _In_ PUCHAR Data, _In_ SIZE_T DataSize);
			}

			VOID Release_Hashed(PCHAR HashBytes);
		}
	}
}


#endif