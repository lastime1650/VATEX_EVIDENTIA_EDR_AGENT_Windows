#ifndef WORKITEM_H
#define WORKITEM_H

#include "util.hpp"

namespace EDR
{
	namespace WORK_ITEM
	{

		typedef struct Event_by_WORKITEM
		{
			EDR::Util::LinkedListEvent::EventType Eventtype;
			EDR::Util::LinkedListEvent::PEventNode Startnode; // Most: 연결리스트 시작주소

		}Event_by_WORKITEM, *PEvent_by_WORKITEM;


		typedef struct WORK_ITEM_STRUCT_ {
			WORK_QUEUE_ITEM WorkItem;

			
			PEvent_by_WORKITEM EventData;

		}WORK_ITEM_STRUCT, *PWORK_ITEM_STRUCT;

		#define WORK_ITEM_ITEM_ALLOC_TAG 'WorK'
		VOID WORK_TIME_QUEUEING(EDR::Util::LinkedListEvent::EventType Eventtype, EDR::Util::LinkedListEvent::PEventNode Startnode);

		VOID WORK_ITEM_ROUTINE(PWORK_ITEM_STRUCT context);






		typedef struct UserEventNode_ {
			CHAR FieldName[25]; // allocated

			EDR::Util::LinkedListEvent::EventDataType DataType;

			PVOID UserAllocatedData; // allocated
			SIZE_T AllocatedDataSize;

			struct UserEventNode_* NextNode;

		}UserEventNode, *PUserEventNode;
		extern "C" VOID Event_Pre_Processing(PEvent_by_WORKITEM Event);
	}
}

#endif