#include "WORK_ITEM.hpp"
#include "IOCTL_Routines.hpp"
#include "APC.hpp"

namespace EDR
{
	namespace WORK_ITEM
	{
		VOID WORK_TIME_QUEUEING(EDR::Util::LinkedListEvent::EventType Eventtype, EDR::Util::LinkedListEvent::PEventNode Startnode)
		{

			PWORK_ITEM_STRUCT Item = (PWORK_ITEM_STRUCT)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(WORK_ITEM_STRUCT), WORK_ITEM_ITEM_ALLOC_TAG);
			if (!Item) return;

			Item->EventData = (PEvent_by_WORKITEM)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(Event_by_WORKITEM), WORK_ITEM_ITEM_ALLOC_TAG);
			if (!Item->EventData)
			{
				ExFreePoolWithTag(Item, WORK_ITEM_ITEM_ALLOC_TAG);
				return;
			}
			Item->EventData->Eventtype = Eventtype;
			Item->EventData->Startnode = Startnode;

			ExInitializeWorkItem(
				&Item->WorkItem,
				(PWORKER_THREAD_ROUTINE)WORK_ITEM_ROUTINE,
				Item
			);

			ExQueueWorkItem(
				&Item->WorkItem,   // 큐에 넣을 Work Item
				DelayedWorkQueue       // 큐 종류 (일반적으로 DelayedWorkQueue 사용)
			);
			

		}

		
		VOID WORK_ITEM_ROUTINE(PWORK_ITEM_STRUCT context)
		{
			/*
			*  안전한 작업을 위한 " PASSIVE_LEVEL " 신호용으로 사용한다.
			* 
				WORK ITEM 컨텍스트 에서 StartNode는 그대로 유지한채, 새로 생성된 스레드에서 비동기적 처리.
				나머지는 할당해제
			*/
			HANDLE ThreadHandle = 0;
			PsCreateSystemThread(
				&ThreadHandle,
				THREAD_ALL_ACCESS,
				NULL,
				0,
				NULL,
				(PKSTART_ROUTINE)Event_Pre_Processing,
				context->EventData
			);
			if (ThreadHandle)
			{
				ZwClose(ThreadHandle); // 완전 백그라운드 실행
			}

			
			ExFreePoolWithTag(context, WORK_ITEM_ITEM_ALLOC_TAG);
		}

		VOID Make_User_LinkedList(HANDLE Processhandle, PUserEventNode* StartNode, PUserEventNode* CurrentNode, PCHAR FieldName, EDR::Util::LinkedListEvent::EventDataType DataType, PVOID Data, SIZE_T Data_Size);
		VOID RemoveAllUserNode(HANDLE Processhandle, PUserEventNode StartNode);
		extern "C" VOID Event_Pre_Processing(PEvent_by_WORKITEM Event)
		{


			EDR::Util::LinkedListEvent::EventType Eventtype = Event->Eventtype;
			EDR::Util::LinkedListEvent::PEventNode Startnode = Event->Startnode;

			EDR::Util::LinkedListEvent::PEventNode CurrentNode = Startnode;


			PUserEventNode StartUserNode = NULL;
			PUserEventNode CurrentUserNode = NULL;

			
			PEPROCESS UserEprocess = NULL;
			// IOCTL 유저 프로세스 ID 가져오기
			HANDLE UserPID = User_PID;
			HANDLE ProcessUserHandle = User_HANDLE;
			if (!UserPID) {
				goto Clean;
			}

			// 유저 프로세스 EPROCESS획득
			if (!NT_SUCCESS( PsLookupProcessByProcessId(UserPID, &UserEprocess) ) || !UserEprocess) {
				goto Clean;
			}
			
			
			
			
			/*
				어떻게 유저모드에게 연결리스트를 제공할 것인가? 

				스택연결하면서 노드를 이어주고,
				ZwVirtualAlloc.. API로 메모리 할당하면 된다.

			*/

			

			KAPC_STATE state;
			KeStackAttachProcess(UserEprocess, &state);

			while (CurrentNode)
			{
				switch (CurrentNode->DataType)
				{
					case EDR::Util::LinkedListEvent::processid:
					{
						PCHAR FieldName = CurrentNode->name; // example Process(Self) or Parent_ProcessId
						HANDLE ProcessId = CurrentNode->DataSize == sizeof(HANDLE) ? *((PHANDLE)CurrentNode->Data) : 0;
						HANDLE ProcessHandle = NULL;
						EDR::Util::Process::Handle::LookupProcessHandlebyProcessId(ProcessId, &ProcessHandle);
						
						/*
							<< 과정 >>
							Process ID ->
							1. Process 절대경로 ->
							2. Process 바이너리 읽기 ->
							3. SHA256 구하기
							4. (2), (1) 동적할당 해제

							<< 삽입할 노드 >>
							1. ProcessId
							2. ProcessImagePath
							3. SHA256

						*/
						// 1. 절대경로 구하기
						PUNICODE_STRING ImagePath = NULL;
						EDR::Util::Process::ImagePath::LookupProcessAbsoluteImagePathbyProcessHandle(ProcessHandle, &ImagePath);
						if (!ImagePath)
						{
							ZwClose(ProcessHandle);
							KeUnstackDetachProcess(&state);
							goto Clean;
						}
							

						// 2. 파일 읽기
						PUCHAR FileBytes = NULL;
						SIZE_T FileSize = 0;
						EDR::Util::File::Read::ReadFile(*ImagePath, &FileBytes, &FileSize);
						if (!FileBytes)
						{
							ZwClose(ProcessHandle);
							EDR::Util::Process::ImagePath::ReleaseLookupProcessAbsoluteImagePathbyProcessHandle(ImagePath);
							KeUnstackDetachProcess(&state);
							goto Clean;
						}

						// 3 SHA256구하기
						PCHAR SHA256 = NULL;
						ULONG SHA256_Size = EDR::Util::Hash::SHA256::SHA256_Hasing(&SHA256, FileBytes, FileSize);
						if(!SHA256)
						{
							ZwClose(ProcessHandle);
							EDR::Util::Process::ImagePath::ReleaseLookupProcessAbsoluteImagePathbyProcessHandle(ImagePath);
							EDR::Util::File::Release_File(FileBytes);
							KeUnstackDetachProcess(&state);
							goto Clean;
						}

						/*
							유저모드에게 연결리스트 생성
						*/
						Make_User_LinkedList(ProcessUserHandle, &StartUserNode, &CurrentUserNode, FieldName, EDR::Util::LinkedListEvent::processid, &ProcessId, sizeof(ProcessId)); // ProcessId
						Make_User_LinkedList(ProcessUserHandle, &StartUserNode, &CurrentUserNode, (PCHAR)"ImagePath", EDR::Util::LinkedListEvent::wstring, &ImagePath->Buffer, ImagePath->MaximumLength); // Image Path
						Make_User_LinkedList(ProcessUserHandle, &StartUserNode, &CurrentUserNode, (PCHAR)"sha256", EDR::Util::LinkedListEvent::astring, (PVOID)SHA256, SHA256_Size); // SHA256


						/*
							Release
						*/
						ZwClose(ProcessHandle);
						EDR::Util::Process::ImagePath::ReleaseLookupProcessAbsoluteImagePathbyProcessHandle(ImagePath);
						EDR::Util::File::Release_File(FileBytes);
						EDR::Util::Hash::Release_Hashed(SHA256);

						break;
					}
					case EDR::Util::LinkedListEvent::filepath:
					{
						/* 이때 파일 경로는 PWCH 타입이다.  */
						/*
							<< 과정 >>
							Process ID ->
							1. 파일 경로
							2. 파일 SHA256

							<< 삽입할 노드 >>
							1. FilePath
							2. SHA256

						*/
						PCHAR FieldName = CurrentNode->name;

						UNICODE_STRING FilePath;
						FilePath.Buffer = (PWCH)CurrentNode->Data;
						FilePath.MaximumLength = (USHORT)CurrentNode->DataSize;
						FilePath.Length = (USHORT)CurrentNode->DataSize;

						PUCHAR FileBytes = NULL;
						SIZE_T FileSize = 0;
						EDR::Util::File::Read::ReadFile(FilePath, &FileBytes, &FileSize);
						if (!FileBytes)
						{
							KeUnstackDetachProcess(&state);
							goto Clean;
						}
							

						PCHAR SHA256 = NULL;
						ULONG SHA256_Size = EDR::Util::Hash::SHA256::SHA256_Hasing(&SHA256, FileBytes, FileSize);
						if (!SHA256)
						{
							EDR::Util::File::Release_File(FileBytes);
							KeUnstackDetachProcess(&state);
							goto Clean;
						}

						Make_User_LinkedList(ProcessUserHandle, &StartUserNode, &CurrentUserNode, FieldName, EDR::Util::LinkedListEvent::wstring, &CurrentNode->Data, CurrentNode->DataSize); // FilePath
						Make_User_LinkedList(ProcessUserHandle, &StartUserNode, &CurrentUserNode, (PCHAR)"sha256", EDR::Util::LinkedListEvent::astring, (PVOID)SHA256, SHA256_Size); // SHA256


						/*
							Release
						*/
						EDR::Util::File::Release_File(FileBytes);
						EDR::Util::Hash::Release_Hashed(SHA256);

						break;
					}
					default:
					{
						// 그대로 진행
						Make_User_LinkedList(ProcessUserHandle, &StartUserNode, &CurrentUserNode, CurrentNode->name, CurrentNode->DataType, (PVOID)CurrentNode->Data, CurrentNode->DataSize); // Else ex) timestamp, just number or string
						break;
					}
				}

				CurrentNode = CurrentNode->NextNode;
			}
			KeUnstackDetachProcess(&state);


			/*
				User에게 전달 !!! 
			*/
			EDR::APC::ApcToUser(Eventtype, (PVOID)StartUserNode);
			/*
				StartUserNode 해제는 유저모드에서 처리한다.
			*/
			goto Clean;

			/*
			Err:
				{

					// 중간에 실패하는 경우...
					if (Startnode)
						RemoveAllUserNode(ProcessUserHandle, StartUserNode);

					goto Clean;
				}*/
		Clean:
			{

				if (UserEprocess)
					ObDereferenceObject(UserEprocess);

				EDR::Util::LinkedListEvent::RemoveAllEventNodes(Event->Startnode);
				ExFreePoolWithTag(Event, WORK_ITEM_ITEM_ALLOC_TAG);
			}
		}


		PUserEventNode CreateUserNode(HANDLE Processhandle, PCHAR FieldName, EDR::Util::LinkedListEvent::EventDataType DataType, PVOID Data, SIZE_T Data_Size);
		VOID Make_User_LinkedList(HANDLE Processhandle, PUserEventNode* StartNode, PUserEventNode* CurrentNode, PCHAR FieldName, EDR::Util::LinkedListEvent::EventDataType DataType, PVOID Data, SIZE_T Data_Size)
		{
			/*
			****************************************************************************
				이 코드는 절대적으로 유저모드 스택에 연결되고 있어야한다. ( Connected UserMode Stack in here ) 
			****************************************************************************
			*/

			if (!*StartNode)
			{
				// Init
				*StartNode = CreateUserNode(Processhandle, FieldName, DataType, Data, Data_Size);
				*CurrentNode = *StartNode;
			}
			else
			{
				// Append
				PUserEventNode New = CreateUserNode(Processhandle, FieldName, DataType, Data, Data_Size);
				((PUserEventNode)*CurrentNode)->NextNode = New;
				*CurrentNode = New;

			}

		}

		PUserEventNode CreateUserNode(HANDLE Processhandle, PCHAR FieldName, EDR::Util::LinkedListEvent::EventDataType DataType, PVOID Data, SIZE_T Data_Size)
		{

			PUserEventNode NewNode = NULL;

			PVOID tmp_UserEventNode = NULL;
			SIZE_T tmp_UserEventNode_size = sizeof(UserEventNode);
			if(!NT_SUCCESS( EDR::Util::User::Memory::AllocateMemory(Processhandle, &tmp_UserEventNode, &tmp_UserEventNode_size)) )
				return NULL;

			NewNode = (PUserEventNode)tmp_UserEventNode;
			RtlZeroMemory(NewNode, sizeof(UserEventNode));

			// Data 영역또한 할당.
			PVOID tmp_UserEventData = NULL;
			SIZE_T tmp_UserEventData_size = Data_Size;
			if (!NT_SUCCESS(EDR::Util::User::Memory::AllocateMemory(Processhandle, &tmp_UserEventData, &tmp_UserEventData_size)))
			{
				ZwFreeVirtualMemory(Processhandle, &tmp_UserEventNode, &tmp_UserEventNode_size, MEM_FREE);
				return NULL;
			}
			PVOID allocated_data = tmp_UserEventData;
			NewNode->UserAllocatedData = allocated_data;
			NewNode->AllocatedDataSize = Data_Size;
			RtlCopyMemory(NewNode->UserAllocatedData, Data, Data_Size);


			RtlCopyMemory(NewNode->FieldName, FieldName, (strlen(FieldName) + 1) > 25 ? 24 : (strlen(FieldName) + 1)); // 25 초과하면 24길이까지 복사하고, 나머지는 NULL


			NewNode->DataType = DataType;


			NewNode->NextNode = NULL;

			return NewNode;
		}

		VOID RemoveAllUserNode(HANDLE Processhandle, PUserEventNode StartNode)
		{
			PVOID tmp;
			SIZE_T tmp_size;

			PUserEventNode Current = StartNode;
			while (Current)
			{

				/*
					데이터 삭제
				*/
				tmp = Current->UserAllocatedData;
				tmp_size = Current->AllocatedDataSize;
				ZwFreeVirtualMemory(Processhandle, &tmp, &tmp_size, MEM_FREE);

				/*
					노드 삭제
				*/
				PUserEventNode NextNode = Current->NextNode;

				tmp = (PVOID)Current;
				tmp_size = sizeof(UserEventNode);

				ZwFreeVirtualMemory(Processhandle, &tmp, &tmp_size, MEM_FREE);

				Current = NextNode;
			}
		}
	}
	
}
