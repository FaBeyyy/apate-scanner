#include "EasyMem.hpp"
#include <fstream>

EasyMem::EasyMem()
{

}

EasyMem::~EasyMem()
{
	CloseHandle(hProcess);
}

//Getter & Setter
HANDLE EasyMem::getProcessHandle()
{
	if (this->hProcess == nullptr)
	{
		throw(std::exception(""));
	}
	return this->hProcess;
}

DWORD EasyMem::getProcessId()
{
	if (this->processId == 0)
	{
		throw(std::invalid_argument(""));
	}
	return this->processId;
}

void EasyMem::setProcessHandle(HANDLE hProcess)
{
	if (hProcess == nullptr)
	{
		throw(std::exception(""));
	}
	this->hProcess = hProcess;
}

bool EasyMem::openProcessHandle(String processName, DWORD access)
{
	processId = ProcessHelper::getProcessIdFromName(processName);
	if (processId == 0)
	{
		throw(std::exception(""));
		return false;
	}
	auto processHandle = OpenProcess(access, false, processId);
	if (processHandle == nullptr)
	{
		throw(std::exception(""));
		return false;
	}
	hProcess = processHandle;
	return true;
}

bool EasyMem::openProcessHandle(DWORD processId, DWORD access)
{
	if (processId == 0)
	{
		throw(std::exception(""));
	}
	this->processId = processId;
	auto processHandle = OpenProcess(access, false, this->processId);
	if (processHandle == INVALID_HANDLE_VALUE)
	{
		throw(std::exception(""));
	}
	hProcess = processHandle;
	return true;
}

//ProcessHelper Functions
DWORD ProcessHelper::getProcessIdFromName(String processName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	DWORD processId = 0;

	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snapshot == nullptr)
	{
		throw(std::exception(""));
	}

	if (Process32First(snapshot, &entry) == true)
	{
		while (Process32Next(snapshot, &entry) == true)
		{
			String snapShotName = entry.szExeFile;
			if (snapShotName == processName)
			{
				processId = entry.th32ProcessID;
				CloseHandle(snapshot);
			}
		}
	}
	return processId;
}

bool ProcessHelper::isRunning(String processName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	DWORD processId = 0;

	while (processId == 0)
	{
		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	
		if (snapshot == nullptr)
		{
	
			return false;
		}
		if (Process32First(snapshot, &entry) == true)
		{
			while (Process32Next(snapshot, &entry) == true)
			{
				
				String snapShotName = entry.szExeFile;
	
				if (snapShotName == processName)
				{
		
					processId = entry.th32ProcessID;
			
					CloseHandle(snapshot);
					Sleep(100);
					break;
				}
			}
		}
		Sleep(100);
	}
	return true;
}
