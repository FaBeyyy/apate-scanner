#include <Windows.h>
#include <iostream>
#include "HandleFunctions.h"
#include "Module.h"
#include <fstream>
#include "PipeClient.h"
#include "Pointerscanner.h"
#include "json.hpp"

using json = nlohmann::json;
void to_json(json& j, const MemoryEntry& e) {
	j = json{ {"address", e.address}, {"sizeInBytes", e.sizeInBytes}, {"value", e.value} };
}

void to_json(json& j, const PointerEntry& e) {
	j = json{ {"baseAddress", e.baseAddress}, {"baseOffset", e.baseOffset}, {"offsets", e.offsets}, {"pointsTo", e.pointsTo}, {"value", e.value} };
}

void from_json(const json& j, PointerEntry& p) {
	j.at("baseAddress").get_to(p.baseAddress);
	j.at("baseOffset").get_to(p.baseOffset);
	j.at("offsets").get_to(p.offsets);
	j.at("pointsTo").get_to(p.pointsTo);
	j.at("value").get_to(p.value);
}


void TestWriteFile(std::string a)
{
	std::ofstream myfile("test1337.txt", std::ios::out | std::ios::app);
	if (myfile.is_open())
	{
		myfile << a << std::endl;
	}
}



void main() {
	try {
		TestWriteFile("0");
		std::string processName = "ModernWarfare.exe";
		//std::string processName = "iw4mp.exe";
		auto memory = std::make_shared<EasyMem>();
		bool isRunning = ProcessHelper::isRunning(processName); //dt vector pls fix
		TestWriteFile("1");
		Sleep(60000);
		auto handle = getHandle(processName.c_str());
		TestWriteFile("2" + std::to_string((int)handle));
		if (!handle || handle == nullptr || handle == INVALID_HANDLE_VALUE) throw std::exception();
		TestWriteFile("3");
		auto module = std::make_shared<Module>(handle, processName);
		memory->setProcessHandle(handle);
		TestWriteFile("before loop: handle: " + std::to_string((int)handle));
		std::string pipeName = "\\\\.\\pipe\\mypipe1337";
		auto pipe = std::make_shared<PipeClient>(pipeName);
		auto pointerscanner = std::make_shared<Pointerscanner>(memory, pipe, module);

		while (true)
		{
		
				DWORD exitCode;
				GetExitCodeProcess(memory->getProcessHandle(), &exitCode);
				if (exitCode == STILL_ACTIVE) {
					
					auto pipeDataBuffer = pipe->readPipe();
				
				
					auto pipeData = json::parse(pipeDataBuffer);
					if (pipeDataBuffer != "") {
					
						auto type = pipeData["type"];
						
						auto payload = json(pipeData["payload"]);
						
						std::vector<MemoryEntry> addresses;
						std::vector<PointerEntry> pointers;

						auto returnData = json();
						returnData["type"] = pipeData["type"];
						

						if (type == "SEARCH_INITIAL_VALUE") {
							addresses = pointerscanner->searchInitialValue(payload);
							returnData["payload"] = addresses;
						}

						if (type == "SEARCH_NEXT_VALUE") {
							addresses = pointerscanner->searchNextValue(payload);
							returnData["payload"] = addresses;
						}

						if (type == "SEARCH_POINTERS") {
							pointers = pointerscanner->searchStaticPointers(payload);
							returnData["payload"]["pointerEntries"] = pointers;
							returnData["payload"]["shouldPointTo"] = payload;
						}

						if (type == "UPDATE_POINTERS") {
							pointers = pointerscanner->updatePointers(payload);
							returnData["payload"]["pointerEntries"] = pointers;
							returnData["payload"]["shouldPointTo"] = 0;
						}

						pipe->writePipe(returnData.dump());
						
					}
					else {
						//update existing addresses
					/*	auto returnData = json();
						returnData["type"] = pipeData["type"];
						auto addresses = pointerscanner->updateCurrentEntries();
						returnData["payload"] = addresses;
						pipe->writePipe(returnData.dump());*/

					}
				}
				else
				{
					break;
				}
				Sleep(10);
		}
	}
	catch (std::exception &e) {
		TestWriteFile("exc" + std::to_string(GetLastError()) + std::string(e.what()));
	}
	TerminateThread(GetCurrentThread(), 1458);
}

	BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
	{
		if (dwReason == DLL_PROCESS_ATTACH) {
			DisableThreadLibraryCalls(hModule);		//disables attach and detach notifications
			auto hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);	
			DWORD exitCode;

		}
		else if (dwReason == DLL_PROCESS_DETACH) {

		}
		return TRUE;
	}