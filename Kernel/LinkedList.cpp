#include "LinkedList.hpp"

namespace EDR
{
	namespace Util
	{
		namespace LinkedListEvent
		{
			PEventNode CreateEventNode(PCHAR name_p, SIZE_T name_Maxlen, EventDataType DataType, PUCHAR Data, SIZE_T DataSize)
			{
				PEventNode NewNode = (PEventNode)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(EventNode), EventNode_Data_ALLOC_TAG);
				if (!NewNode)
					return NULL;

				NewNode->name = (PCHAR)ExAllocatePool2(POOL_FLAG_NON_PAGED, name_Maxlen, EventNode_Data_ALLOC_TAG);
				if (!NewNode->name)
				{
					ExFreePoolWithTag(NewNode, EventNode_Data_ALLOC_TAG);
					return NULL;
				}
				RtlCopyMemory(NewNode->name, name_p, name_Maxlen);

				NewNode->Data = (PUCHAR)ExAllocatePool2(POOL_FLAG_NON_PAGED, DataSize, EventNode_Data_ALLOC_TAG);
				if (!NewNode->Data)
				{
					ExFreePoolWithTag(NewNode->name, EventNode_Data_ALLOC_TAG);
					ExFreePoolWithTag(NewNode, EventNode_Data_ALLOC_TAG);
					return NULL;
				}
				RtlCopyMemory(NewNode->Data, Data, DataSize);

				NewNode->DataSize = DataSize;

				NewNode->DataType = DataType;

				NewNode->NextNode = NULL;

				return NewNode;
			}

			BOOLEAN AppendEventNode(PEventNode* CurrentNode, PCHAR name_p, SIZE_T name_Maxlen, EventDataType DataType, PUCHAR Data, SIZE_T DataSize)
			{
				if (!CurrentNode || !*CurrentNode)
					return FALSE;

				PEventNode NewNode = CreateEventNode(name_p, name_Maxlen, DataType, Data, DataSize);
				(*CurrentNode)->NextNode = NewNode;

				*CurrentNode = NewNode;

				return TRUE ;

			}

			VOID RemoveAllEventNodes(PEventNode StartNode)
			{
				if (!StartNode)
					return;

				PEventNode Current = StartNode;
				while (Current)
				{

					ExFreePoolWithTag(Current->name, EventNode_Data_ALLOC_TAG);

					PEventNode nextnode = Current->NextNode;
					ExFreePoolWithTag(Current, EventNode_Data_ALLOC_TAG);

					Current = nextnode;
				}

				return;
			}

		}
	}
}