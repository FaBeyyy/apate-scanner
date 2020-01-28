#include <Windows.h>
#include <iostream>
#include "HandleFunctions.h"
#include "Module.h"
#include <fstream>
#include "PipeClient.h"
#include "json.hpp"
#include "Enum.hpp"
#include "PointerScannerHelperFunctions.h"

using json = nlohmann::json;
using namespace PointerScannerHelperFunctions;

template <class T>
void to_json(json& j, const MemoryEntry<T>& e) {
	j = json{ {"address", e.address}, {"sizeInBytes", e.sizeInBytes}, {"value", e.value} };
}
template <class T>
void to_json(json& j, const PointerEntry<T>& e) {
	j = json{ {"baseAddress", e.baseAddress}, {"baseOffset", e.baseOffset}, {"offsets", e.offsets}, {"pointsTo", e.pointsTo}, {"value", e.value} };
}
template <class T>
void from_json(const json& j, PointerEntry<T>& p) {
	j.at("baseAddress").get_to(p.baseAddress);
	j.at("baseOffset").get_to(p.baseOffset);
	j.at("offsets").get_to(p.offsets);
	j.at("pointsTo").get_to(p.pointsTo);
	j.at("value").get_to(p.value);
}
template <class T>
void from_json(const json& j, ValueSearchPayloadTyped<T>& p) {
	j.at("value").get_to(p.value);
	j.at("dataType").get_to(p.dataType);
	j.at("compareType").get_to(p.compareType);
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
		auto pointerscannerInt = std::make_shared<Pointerscanner<int>>(memory, pipe, module, DataType::INT_TYPE);
		auto pointerscannerFloat = std::make_shared<Pointerscanner<float>>(memory, pipe, module, DataType::FLOAT_TYPE);
		auto pointerscannerBool = std::make_shared<Pointerscanner<bool>>(memory, pipe, module, DataType::BOOL_TYPE);
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
						
						auto returnData = json();
						returnData["type"] = pipeData["type"];
						

						if (type == "SEARCH_INITIAL_VALUE" || type == "SEARCH_NEXT_VALUE") {
							auto searchPayloadString = payload;
							std::string dataTypeString = payload["dataType"];
							if (dataTypeString == magic_enum::enum_name(DataType::INT_TYPE)) {
								returnData["payload"] = getAddressesFromScanType<int>(type, pointerscannerInt, payload);
							}
							else if (dataTypeString == magic_enum::enum_name(DataType::FLOAT_TYPE)) {
								returnData["payload"] = getAddressesFromScanType<float>(type, pointerscannerFloat, payload);
							}
							else if (dataTypeString == magic_enum::enum_name(DataType::BOOL_TYPE)) {
								returnData["payload"] = getAddressesFromScanType<bool>(type, pointerscannerBool, payload);
							}
						}

						if (type == "SEARCH_POINTERS") {
							auto pointers = pointerscannerInt->searchStaticPointers(payload);
							returnData["payload"]["pointerEntries"] = pointers;
							returnData["payload"]["shouldPointTo"] = payload;
						}

						if (type == "UPDATE_POINTERS") {
							auto pointers = pointerscannerInt->updatePointers(payload);
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