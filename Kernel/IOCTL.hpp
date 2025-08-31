#ifndef IOCTL_H
#define IOCTL_H

#include <ntifs.h>

namespace EDR
{
	namespace Util
	{
		namespace IOCTL
		{
			extern PDEVICE_OBJECT DeviceObject;

			#define IOCTL_DeviceName L"\\Device\\VATEX_EVIDENTIA_EDR_AGENT"
			#define IOCTL_Device_SymbolicName L"\\??\\VATEX_EVIDENTIA_EDR_AGENT"
			NTSTATUS BuildIOCTL(PDRIVER_OBJECT DriverObject);

		}
	}
}

#endif