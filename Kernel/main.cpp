#include <ntifs.h>

#include "IOCTL.hpp"
#include "NotifyRoutine_Process.hpp"

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING registry_path)
{

	UNREFERENCED_PARAMETER(registry_path);

	// IOCTL ����
	EDR::Util::IOCTL::BuildIOCTL(driver);

	// �ݹ��Լ� ����
	

	return EDR::NotifyRoutines::ProcessCreate::Set_NotifyRoutine_ProcessCreate();// 
}