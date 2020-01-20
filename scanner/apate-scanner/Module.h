#pragma once
#include <Windows.h>
#include <iostream>

class Module {
public:
	DWORD baseModule = 0;
	HMODULE baseModuleRaw;
	Module(HANDLE hProcess, std::string moduleName);
};
