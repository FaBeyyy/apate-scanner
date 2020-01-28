#include "Headers.h"
#include <Psapi.h>

NTSTATUS enumerate_handles(ENUM_HANDLE_CALLBACK callback)
{
	NTSTATUS                    status = STATUS_UNSUCCESSFUL;
	PVOID                       buffer = NULL;
	ULONG                       bufferSize = 0;

	do {
		status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)16/*SystemHandleInformation*/, buffer, bufferSize, &bufferSize);
		if (!NT_SUCCESS(status)) {
			if (status == STATUS_INFO_LENGTH_MISMATCH) {
				if (buffer != NULL)
					VirtualFree(buffer, 0, MEM_RELEASE);
				buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
				continue;
			}
			break;
		}
		else {
			PSYSTEM_HANDLE_INFORMATION handleInfo = (PSYSTEM_HANDLE_INFORMATION)buffer;
			for (ULONG i = 0; i < handleInfo->NumberOfHandles; i++) {
				auto handle = &handleInfo->Handles[i];
				status = callback(handle);
				if (NT_SUCCESS(status))
					break;
			}
			break;
		}
	} while (true);
	if (buffer != NULL)
		VirtualFree(buffer, 0, MEM_RELEASE);
	return status;
}

void print_handle_information()
{
	enumerate_handles([](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) {

		if (GetCurrentProcessId() != handle->UniqueProcessId) return STATUS_UNSUCCESSFUL;

		printf("Handle Object:  0x%p\n", handle->Object);
		printf("Handle Value:   0x%04X\n", handle->HandleValue);
		printf("Granted Access: 0x%08X\n", handle->GrantedAccess);

		//
		// Print name information
		// 
		PVOID       buffer = NULL;
		ULONG       bufferSize = 0x100;
		NTSTATUS    status;

		do {
			buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
			status = NtQueryObject((HANDLE)handle->HandleValue, (OBJECT_INFORMATION_CLASS)1/*ObjectNameInformation*/, buffer, bufferSize, &bufferSize);
			if (NT_SUCCESS(status)) break;
			if (buffer != NULL)
				VirtualFree(buffer, 0, MEM_RELEASE);
		} while (status == STATUS_INFO_LENGTH_MISMATCH);

		if (!NT_SUCCESS(status))
			goto EXIT;

		printf("Name:           %ws\n", ((POBJECT_NAME_INFORMATION)buffer)->Name.Buffer);

		VirtualFree(buffer, 0, MEM_RELEASE);

		do {
			buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
			status = NtQueryObject((HANDLE)handle->HandleValue, ObjectTypeInformation, buffer, bufferSize, &bufferSize);
			if (NT_SUCCESS(status)) break;
			if (buffer != NULL)
				VirtualFree(buffer, 0, MEM_RELEASE);
		} while (status == STATUS_INFO_LENGTH_MISMATCH);

		if (!NT_SUCCESS(status))
			goto EXIT;

		printf("Type:           %ws\n", ((POBJECT_TYPE_INFORMATION)buffer)->TypeName.Buffer);

		//
		// 7 is the process type index on my system. It's possible that this is different value on your system.
		// You can compare the TypeName to "Process" instead of doing it like this
		//
		if (handle->ObjectTypeIndex == 7) {
			WCHAR processPath[MAX_PATH];
			//
			// The handle needs to have PROCESS_QUERY_INFORMATION and PROCESS_VM_READ
			// access, otherwise this call fails
			// 
			if (GetModuleFileNameExW((HANDLE)handle->HandleValue, NULL, processPath, MAX_PATH))
				printf("Process:        %ws\n", PathFindFileNameW(processPath));
		}
		printf("-------------------------------------------------\n");

	EXIT:
		if (buffer != NULL)
			VirtualFree(buffer, 0, MEM_RELEASE);

		return STATUS_UNSUCCESSFUL;
	});
}