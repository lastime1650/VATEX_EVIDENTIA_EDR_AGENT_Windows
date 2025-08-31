#include "IOCTL.hpp"
#include "IOCTL_Routines.hpp"

namespace EDR
{
	namespace Util
	{
		namespace IOCTL
		{
			PDEVICE_OBJECT DeviceObject = NULL;

			PDEVICE_OBJECT CreateDevice(_In_ PDRIVER_OBJECT DriverObject);
			extern "C" NTSTATUS RequiredRoutine(PDEVICE_OBJECT pDeviceObject, PIRP Irp);
			extern "C" NTSTATUS IOCTLRoutine(PDEVICE_OBJECT pDeviceObject, PIRP Irp);

			NTSTATUS BuildIOCTL(PDRIVER_OBJECT DriverObject)
			{

				DeviceObject = CreateDevice(DriverObject);
				if (!DeviceObject)
					return STATUS_UNSUCCESSFUL;

				// STEP 2) Set DriverObject Dispatch Routines
				DriverObject->MajorFunction[IRP_MJ_CREATE] = RequiredRoutine; // Just required ..
				DriverObject->MajorFunction[IRP_MJ_CLOSE] = RequiredRoutine; // Just required ..

				DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IOCTLRoutine; // IOCTL ! 

				// Final STEP) 디바이스 활성화
				DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

				return STATUS_SUCCESS;

			}


			extern "C" NTSTATUS IOCTLRoutine(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
			{
				UNREFERENCED_PARAMETER(pDeviceObject);

				NTSTATUS status = STATUS_SUCCESS;
				PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp); // Request Information

				switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
				{
				case IOCTL_INIT:
				{
					/*
						유저모드와 최초로 연결될 때,
						** 유저모드 프로그램이 변경되면 재 호출될 수 있다.
					*/
					EDR::Util::IOCTL::Initialize::IOCTL_STRUCT::PIOCTL_INIT_STRUCT Parameter = (EDR::Util::IOCTL::Initialize::IOCTL_STRUCT::PIOCTL_INIT_STRUCT)Irp->AssociatedIrp.SystemBuffer; // Get System Buffer
					ULONG Parameter_Size = irpSp->Parameters.DeviceIoControl.InputBufferLength; // Get Input Buffer Length

					Parameter->output = Initialize::Initialize(Parameter);

					Irp->IoStatus.Status = status;
					Irp->IoStatus.Information = Parameter_Size;
				}
					default:
					{
						Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
						Irp->IoStatus.Information = 0;
					}
				}

				return status;
			}









			extern "C" NTSTATUS RequiredRoutine(PDEVICE_OBJECT pDeviceObject, PIRP Irp) {
				UNREFERENCED_PARAMETER(pDeviceObject);
				UNREFERENCED_PARAMETER(Irp);

				// NOTHING TO DO !!!!

				return STATUS_SUCCESS;
			}

			// 디바이스 생성
			PDEVICE_OBJECT CreateDevice(
				_In_ PDRIVER_OBJECT DriverObject
			) {
				if (!DriverObject)
					return NULL;

				DeviceObject = NULL;
				UNICODE_STRING DeviceName;
				UNICODE_STRING SymbolicLinkName;
				NTSTATUS status;

				RtlInitUnicodeString(&DeviceName, IOCTL_DeviceName);
				RtlInitUnicodeString(&SymbolicLinkName, IOCTL_Device_SymbolicName);

				status = IoCreateDevice(
					DriverObject,
					0,
					&DeviceName,
					FILE_DEVICE_UNKNOWN,
					FILE_DEVICE_SECURE_OPEN,
					FALSE,
					&DeviceObject
				);

				if (NT_SUCCESS(status)) {
					status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
					if (!NT_SUCCESS(status)) {
						IoDeleteDevice(DeviceObject);
						return NULL;
					}
				}

				return DeviceObject;
			}
		}
	}
}