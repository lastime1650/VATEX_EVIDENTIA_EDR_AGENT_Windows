#include <ntifs.h>

#include "IOCTL.hpp"
#include "NotifyRoutine_Process.hpp"

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING registry_path)
{

	UNREFERENCED_PARAMETER(registry_path);

	// IOCTL 설정
	EDR::Util::IOCTL::BuildIOCTL(driver);

	// 콜백함수 설정
	

	return EDR::NotifyRoutines::ProcessCreate::Set_NotifyRoutine_ProcessCreate();// 
}