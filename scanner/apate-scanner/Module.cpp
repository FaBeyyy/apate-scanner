#include "Module.h"
#include "Enumeration.h"
#include <fstream>
#include <string>

Module::Module(HANDLE hProcess, std::string moduleName) {
	HMODULE hMods[1024];
	DWORD cb;
	DWORD flag;

	while (this->baseModule == 0)
	{
		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cb))
		{
			char szModName[MAX_PATH] = { NULL };
			for (int i = 0; i < cb / sizeof(HMODULE); i++)
			{
				if (GetModuleBaseNameA(hProcess, hMods[i], szModName, MAX_PATH))
				{
					
					if (!strcmp(szModName, moduleName.c_str()))
					{
						this->baseModule = (DWORD)hMods[i];
						this->baseModuleRaw = hMods[i];
						break;
					}
				}
			}
		}
		Sleep(1000);
	}
}