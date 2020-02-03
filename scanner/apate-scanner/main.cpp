#include <Windows.h>
#include <iostream>
#include "HandleFunctions.h"
#include "Module.h"
#include <fstream>
#include "PipeClient.h"
#include "json.hpp"
#include "Enum.hpp"
#include "PointerScannerHelperFunctions.hpp"
#include <variant>
#include <any>

using json = nlohmann::json;
using namespace PointerScannerHelperFunctions;

template <class T>
void to_json(json& j, const MemoryEntry<T>& e) {
	j = json{ {"address", e.address}, {"dataType", magic_enum::enum_name(e.dataType)}, {"value", e.value} };
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

void TestWriteFilet(int a)
{
	std::ofstream myfile("test1337.txt", std::ios::out | std::ios::app);
	if (myfile.is_open())
	{
		myfile << std::to_string(a) << std::endl;
	}
}


void to_json(json& j, const MemoryEntry<std::any>& e) {
	auto dataType = e.dataType;
	if (dataType == DataType::INT_TYPE) {
		auto value = std::any_cast<int>(e.value);
		j = json{ {"address", e.address}, {"dataType", magic_enum::enum_name(e.dataType)}, {"value", value} };
	}
	if (dataType == DataType::FLOAT_TYPE) {
		auto value = std::any_cast<float>(e.value);
		j = json{ {"address", e.address}, {"dataType", magic_enum::enum_name(e.dataType)}, {"value", value} };
	}
	if (dataType == DataType::BOOL_TYPE) {
		auto value = std::any_cast<bool>(e.value);
		j = json{ {"address", e.address}, {"dataType", magic_enum::enum_name(e.dataType)}, {"value", value} };
	}
	
}

void from_json(const json& j, MemoryEntry<std::any>& p) {
	auto dataType = j.at("dataType").get<std::string>();
	
	auto dataTypeEnumContainer = magic_enum::enum_cast<DataType>(dataType);

	auto dataTypeEnumNumber = dataTypeEnumContainer.value();

	p.dataType = dataTypeEnumNumber;
	j.at("address").get_to(p.address);

	if (dataTypeEnumNumber == DataType::INT_TYPE) {
		p.value =  j.at("value").get<int>();
	}
	if (dataTypeEnumNumber == DataType::FLOAT_TYPE) {
		p.value = j.at("value").get<float>();
	}
	if (dataTypeEnumNumber == DataType::BOOL_TYPE) {
		p.value  = j.at("value").get<bool>();
	}
	
}

template <class T>
void from_json(const json& j, ValueSearchPayloadTyped<T>& p) {
	auto dataType = j.at("dataType").get<std::string>();
	auto compareType = j.at("compareType").get<std::string>();
	auto dataTypeEnumContainer = magic_enum::enum_cast<DataType>(dataType);
	auto compareTypeEnumContainer = magic_enum::enum_cast<CompareType>(compareType);
	auto dataTypeEnumNumber = dataTypeEnumContainer.value();
	auto compareTypeEnumNumber = compareTypeEnumContainer.value();
	p.dataType = dataTypeEnumNumber;
	p.compareType = compareTypeEnumNumber;
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
	//try {
		TestWriteFile("0");
		std::string processName = "iw4mp.exe";
		auto memory = std::make_shared<EasyMem>();
		bool isRunning = ProcessHelper::isRunning(processName); //dt vector pls fix
		TestWriteFile("1");
		//Sleep(60000);
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
	//}
	//catch (std::exception &e) {
	//		TestWriteFile("exc" + std::to_string(GetLastError()) + std::string(e.what()));
	//	}
		while (true)
		{
			try {
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

						if (type == "UPDATE_ADDRESSES") {
							std::vector<MemoryEntry<std::any>> entries = payload;
							std::vector<MemoryEntry<std::any>> returnEntries;
							for (auto entry : entries) {
								if (entry.dataType == DataType::INT_TYPE) {
									auto updatedEntry = pointerscannerInt->updateAddress(entry);
									returnEntries.push_back(updatedEntry);
								}
								else if (entry.dataType == DataType::FLOAT_TYPE) {
									auto updatedEntry = pointerscannerFloat->updateAddress(entry);
									returnEntries.push_back(updatedEntry);
								}
								else if (entry.dataType == DataType::BOOL_TYPE) {
									auto updatedEntry = pointerscannerBool->updateAddress(entry);
									returnEntries.push_back(updatedEntry);
								}
							}
							if (returnEntries.size() < 1) returnData["payload"] = "[]";
						
							returnData["payload"] = returnEntries;
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

					}
				}
				else
				{
					break;
				}
				Sleep(10);
			}
			catch (std::exception &e) {
						TestWriteFile("exc" + std::to_string(GetLastError()) + std::string(e.what()));
					}
		
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