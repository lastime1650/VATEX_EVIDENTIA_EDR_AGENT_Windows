#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <ntifs.h>

namespace EDR
{
	namespace Util
	{
		namespace LinkedListEvent
		{

			typedef enum EventType
			{
				process_creation,
				network,
				filesystem,
				image_load,
				registry
			}EventType, * PEventType;

			typedef enum EventDataType
			{
				processid,
				filepath,
				number,
				wstring, // pwch
				astring, // pchar
				boolean,
				timestamp // pchar
			}EventDataType, *PEventDataType;

			#define EventNode_Data_ALLOC_TAG 'ENDT'
			typedef struct LinkedListEventNode_ {

				PCHAR name;

				EventDataType DataType;

				PUCHAR Data;
				SIZE_T DataSize;

				struct LinkedListEventNode_* NextNode;

			}EventNode, * PEventNode;

			PEventNode CreateEventNode(PCHAR name_p, SIZE_T name_Maxlen, EventDataType DataType, PUCHAR Data, SIZE_T DataSize);
			BOOLEAN AppendEventNode( _Inout_ PEventNode* CurrentNode, PCHAR name_p, SIZE_T name_Maxlen, EventDataType DataType, PUCHAR Data, SIZE_T DataSize);
			VOID RemoveAllEventNodes(PEventNode StartNode);
		}
	}
}

#endif