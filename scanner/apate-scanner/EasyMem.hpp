#pragma once
#include <Windows.h>
#include <string>
#include <TlHelp32.h>
#ifdef UNICODE
typedef std::wstring String;
#else
typedef std::string String;
#endif

struct ACCESS_RIGHTS
{
	static constexpr DWORD FULL = PROCESS_ALL_ACCESS;
	static constexpr DWORD READ = PROCESS_VM_READ;
	static constexpr DWORD WRITE = PROCESS_VM_WRITE;
};

class EasyMem
{
private:
	HANDLE hProcess = nullptr;
	DWORD processId = 0;


public:
	EasyMem();
	~EasyMem();
	void setProcessHandle(HANDLE hProcess);
	HANDLE getProcessHandle();
	DWORD getProcessId();
	bool openProcessHandle(String processName, DWORD access = ACCESS_RIGHTS::FULL);
	bool openProcessHandle(DWORD processId, DWORD access = ACCESS_RIGHTS::FULL);
	

	template <class T>
	T read(uintptr_t adress)
	{
		T returnBuffer = 0;
		ReadProcessMemory(hProcess, (LPCVOID)adress, &returnBuffer, sizeof(T), 0);
		return returnBuffer;
	}


	template <class T>
	T read64(INT64 adress)
	{
		T returnBuffer = 0;//
		ReadProcessMemory(hProcess, (LPCVOID)adress, &returnBuffer, sizeof(T), 0);
		return returnBuffer;
	}

	template<class T>
	bool write(SIZE_T adress, T value)
	{
		return WriteProcessMemory(hProcess, (LPVOID)adress, &value, sizeof(value), 0);
	}

};

namespace ProcessHelper
{
	DWORD getProcessIdFromName(String processName);
	bool isRunning(String processName);
}

