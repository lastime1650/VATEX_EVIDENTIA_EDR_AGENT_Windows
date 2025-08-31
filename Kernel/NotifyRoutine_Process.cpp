#include "NotifyRoutine_Process.hpp"
#include "WORK_ITEM.hpp"

namespace EDR
{
	namespace NotifyRoutines
	{
		namespace ProcessCreate
		{

			extern "C" VOID ProcessCreateRoutineEx_HANDLER(
				_Inout_ PEPROCESS Process,
				_In_ HANDLE ProcessId,
				_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
			)
			{

				if (PsIsSystemProcess(Process))
					return;

				// Event Log ���Ḯ��Ʈ
				EDR::Util::LinkedListEvent::PEventNode StartNode = NULL;
				EDR::Util::LinkedListEvent::PEventNode CurrentNode = NULL;

				// 1. Ÿ�ӽ�����
				PCHAR Timestamp = NULL;
				ULONG32 Timestamp_size = EDR::Util::Timestamp::Get_Timestamp(&Timestamp);

				// 2. �ڽ� PID
				HANDLE Self_ProcessId = ProcessId;

				BOOLEAN is_create;

				if (CreateInfo)
				{
					/*
						���μ��� ���� ��
					*/
					/*
						���⿡�� ���� ����.
							
							1. Ÿ�ӽ����� ( CHAR )
							2. �ڽ� PID
							3. ��������
							4. �θ� PID
							5. CommandLine
							5. 
					*/

					// 3. ��������
					is_create = TRUE;

					// 4. �θ� PID
					HANDLE Parent_ProcessId = CreateInfo->ParentProcessId;

					// 5. CommandLine
					//PCHAR CommandLine_A = NULL;
					//ULONG32 CommandLine_A_Size = 0;
					//if (!EDR::Util::String::UnicodeString_to_ANSI(CreateInfo->CommandLine, &CommandLine_A, &CommandLine_A_Size))
					//{
					//	goto Err1;
					//}


					// Fin
					StartNode = EDR::Util::LinkedListEvent::CreateEventNode((PCHAR)"timestamp", sizeof("timestamp"), EDR::Util::LinkedListEvent::timestamp, (PUCHAR)Timestamp, Timestamp_size);
					CurrentNode = StartNode;
					EDR::Util::LinkedListEvent::AppendEventNode(&CurrentNode, (PCHAR)"processid", sizeof("processid"), EDR::Util::LinkedListEvent::processid, (PUCHAR)&Self_ProcessId, sizeof(Self_ProcessId));
					EDR::Util::LinkedListEvent::AppendEventNode(&CurrentNode, (PCHAR)"is_created", sizeof("is_created"), EDR::Util::LinkedListEvent::boolean, (PUCHAR)&is_create, sizeof(is_create));
					EDR::Util::LinkedListEvent::AppendEventNode(&CurrentNode, (PCHAR)"parent_processid", sizeof("parent_processid"), EDR::Util::LinkedListEvent::processid, (PUCHAR)&Parent_ProcessId, sizeof(Parent_ProcessId));
					EDR::Util::LinkedListEvent::AppendEventNode(&CurrentNode, (PCHAR)"commandline", sizeof("commandline"), EDR::Util::LinkedListEvent::wstring, (PUCHAR)CreateInfo->CommandLine->Buffer, CreateInfo->CommandLine->MaximumLength);
				}
				else
				{
					/*
						���μ��� ���� ��
					*/
					/*
						���⿡�� ���� ����.

							1. Ÿ�ӽ����� ( CHAR )
							2. �ڽ� PID
							3. ��������
					*/

					// 3. ��������
					is_create = FALSE;

					StartNode = EDR::Util::LinkedListEvent::CreateEventNode((PCHAR)"timestamp", sizeof("timestamp"), EDR::Util::LinkedListEvent::timestamp, (PUCHAR)Timestamp, Timestamp_size);
					CurrentNode = StartNode;
					EDR::Util::LinkedListEvent::AppendEventNode(&CurrentNode, (PCHAR)"processid", sizeof("processid"), EDR::Util::LinkedListEvent::processid, (PUCHAR)&Self_ProcessId, sizeof(Self_ProcessId));
					EDR::Util::LinkedListEvent::AppendEventNode(&CurrentNode, (PCHAR)"is_created", sizeof("is_created"), EDR::Util::LinkedListEvent::boolean, (PUCHAR)&is_create, sizeof(is_create));
				}

				// Work - Item ���
				EDR::WORK_ITEM::WORK_TIME_QUEUEING(
					EDR::Util::LinkedListEvent::process_creation,
					StartNode
				);

				if (Timestamp)
					EDR::Util::Timestamp::Release_Got_Time(Timestamp);

				return;

			
			}



			NTSTATUS Set_NotifyRoutine_ProcessCreate()
			{
				return PsSetCreateProcessNotifyRoutineEx(ProcessCreateRoutineEx_HANDLER, FALSE);
			}

			NTSTATUS UnSet_NotifyRoutine_ProcessCreate()
			{
				return PsSetCreateProcessNotifyRoutineEx(ProcessCreateRoutineEx_HANDLER, TRUE);
			}
		}
	}
}