#include "Enumeration.h"
#include <cstdio>
#include <tlhelp32.h>
#include <iostream>
#include <fstream>
#include <string>

HANDLE get_handle_to_process(LPWSTR process)
{
	HANDLE hProcess = NULL;

	enumerate_handles([&](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) {

		if (GetCurrentProcessId() != handle->UniqueProcessId) return STATUS_UNSUCCESSFUL;

		BOOL        found = FALSE;
		PVOID       buffer = NULL;
		ULONG       bufferSize = 0x100;
		NTSTATUS    status;

		//
		// 7 is the process type index. It's possible that this is different value on your system. 
		// Check the output print_handle_information for more info. You can compare the TypeName to
		// "Process" instead of doing it like this
		//
		if (handle->ObjectTypeIndex == 7) {

			WCHAR processPath[MAX_PATH] = {};
			//
			// The handle needs to have PROCESS_QUERY_INFORMATION and PROCESS_VM_READ
			// access, otherwise this call fails
			// 
			if (GetModuleFileNameExW((HANDLE)handle->HandleValue, NULL, processPath, MAX_PATH)) {
				LPWSTR filename = PathFindFileNameW(processPath);
				if (!wcscmp(filename, process)) {
					found = TRUE;
					hProcess = (HANDLE)handle->HandleValue;
				}
			}
		}

		//
		// STATUS_SUCCESS stops the enumeration
		//
		if (found)
			return STATUS_SUCCESS;

		return STATUS_UNSUCCESSFUL;
	});

	return hProcess;
}//0x00001410

HANDLE getHandle(const char * process)
{
	HANDLE hProcess = NULL;

	enumerate_handles([&](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) {
	
		if (GetCurrentProcessId() != handle->UniqueProcessId) return STATUS_UNSUCCESSFUL;
		
		BOOL        found = FALSE;
		PVOID       buffer = NULL;
		ULONG       bufferSize = 0x100;
		NTSTATUS    status;

		//
		// 7 is the process type index. It's possible that this is different value on your system. 
		// Check the output print_handle_information for more info. You can compare the TypeName to
		// "Process" instead of doing it like this
		//
		if (handle->ObjectTypeIndex == 7) {
		
			CHAR processPath[MAX_PATH] = {};
			//
			// The handle needs to have PROCESS_QUERY_INFORMATION and PROCESS_VM_READ
			// access, otherwise this call fails
			// 
			if (GetModuleFileNameEx((HANDLE)handle->HandleValue, NULL, processPath, MAX_PATH)) {
			
				LPSTR filename = PathFindFileName(processPath);
			
				if (!strcmp(filename, process)) {
			
					found = TRUE;
					hProcess = (HANDLE)handle->HandleValue;
				}
				else {
			
				}
			}
		}

		//
		// STATUS_SUCCESS stops the enumeration
		//
		if (found)
			return STATUS_SUCCESS;

		return STATUS_UNSUCCESSFUL;
	});

	return hProcess;
}//0x00001410

