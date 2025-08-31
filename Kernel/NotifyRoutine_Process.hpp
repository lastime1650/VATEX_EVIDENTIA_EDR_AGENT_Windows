#ifndef NOTIFYROUTINE_PROCESS_H
#define NOTIFYROUTINE_PROCESS_H

#include "util.hpp"

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
			);

			NTSTATUS Set_NotifyRoutine_ProcessCreate();
			NTSTATUS UnSet_NotifyRoutine_ProcessCreate();


			//

			
		}
	}
}

#endif