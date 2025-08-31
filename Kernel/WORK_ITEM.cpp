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
				&Item->WorkItem,   // ť�� ���� Work Item
				DelayedWorkQueue       // ť ���� (�Ϲ������� DelayedWorkQueue ���)
			);
			

		}

		
		VOID WORK_ITEM_ROUTINE(PWORK_ITEM_STRUCT context)
		{
			/*
			*  ������ �۾��� ���� " PASSIVE_LEVEL " ��ȣ������ ����Ѵ�.
			* 
				WORK ITEM ���ؽ�Ʈ ���� StartNode�� �״�� ������ä, ���� ������ �����忡�� �񵿱��� ó��.
				�������� �Ҵ�����
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
				ZwClose(ThreadHandle); // ���� ��׶��� ����
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
			// IOCTL ���� ���μ��� ID ��������
			HANDLE UserPID = User_PID;
			HANDLE ProcessUserHandle = User_HANDLE;
			if (!UserPID) {
				goto Clean;
			}

			// ���� ���μ��� EPROCESSȹ��
			if (!NT_SUCCESS( PsLookupProcessByProcessId(UserPID, &UserEprocess) ) || !UserEprocess) {
				goto Clean;
			}
			
			
			
			
			/*
				��� ������忡�� ���Ḯ��Ʈ�� ������ ���ΰ�? 

				���ÿ����ϸ鼭 ��带 �̾��ְ�,
				ZwVirtualAlloc.. API�� �޸� �Ҵ��ϸ� �ȴ�.

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
							<< ���� >>
							Process ID ->
							1. Process ������ ->
							2. Process ���̳ʸ� �б� ->
							3. SHA256 ���ϱ�
							4. (2), (1) �����Ҵ� ����

							<< ������ ��� >>
							1. ProcessId
							2. ProcessImagePath
							3. SHA256

						*/
						// 1. ������ ���ϱ�
						PUNICODE_STRING ImagePath = NULL;
						EDR::Util::Process::ImagePath::LookupProcessAbsoluteImagePathbyProcessHandle(ProcessHandle, &ImagePath);
						if (!ImagePath)
						{
							ZwClose(ProcessHandle);
							KeUnstackDetachProcess(&state);
							goto Clean;
						}
							

						// 2. ���� �б�
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

						// 3 SHA256���ϱ�
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
							������忡�� ���Ḯ��Ʈ ����
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
						/* �̶� ���� ��δ� PWCH Ÿ���̴�.  */
						/*
							<< ���� >>
							Process ID ->
							1. ���� ���
							2. ���� SHA256

							<< ������ ��� >>
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
						// �״�� ����
						Make_User_LinkedList(ProcessUserHandle, &StartUserNode, &CurrentUserNode, CurrentNode->name, CurrentNode->DataType, (PVOID)CurrentNode->Data, CurrentNode->DataSize); // Else ex) timestamp, just number or string
						break;
					}
				}

				CurrentNode = CurrentNode->NextNode;
			}
			KeUnstackDetachProcess(&state);


			/*
				User���� ���� !!! 
			*/
			EDR::APC::ApcToUser(Eventtype, (PVOID)StartUserNode);
			/*
				StartUserNode ������ ������忡�� ó���Ѵ�.
			*/
			goto Clean;

			/*
			Err:
				{

					// �߰��� �����ϴ� ���...
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
				�� �ڵ�� ���������� ������� ���ÿ� ����ǰ� �־���Ѵ�. ( Connected UserMode Stack in here ) 
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

			// Data �������� �Ҵ�.
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


			RtlCopyMemory(NewNode->FieldName, FieldName, (strlen(FieldName) + 1) > 25 ? 24 : (strlen(FieldName) + 1)); // 25 �ʰ��ϸ� 24���̱��� �����ϰ�, �������� NULL


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
					������ ����
				*/
				tmp = Current->UserAllocatedData;
				tmp_size = Current->AllocatedDataSize;
				ZwFreeVirtualMemory(Processhandle, &tmp, &tmp_size, MEM_FREE);

				/*
					��� ����
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
