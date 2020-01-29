#pragma once
#include "json.hpp"
#include "EasyMem.hpp"
#include "PipeClient.h"
#include "Module.h"
#include <vector>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <windows.h>
#include <memory>
#include <fstream>
#include <sstream>


using json = nlohmann::json;
typedef DWORD ptr_size;  //OR DWORD IF 32-Bit

template <class T>
struct MemoryEntry {
	uintptr_t address;
	int sizeInBytes;
	T value;
};

template <class T>
struct PointerEntry {
	uintptr_t baseAddress;
	uintptr_t baseOffset;
	std::vector<uintptr_t>offsets;
	uintptr_t pointsTo;
	T value;
};

struct ValueSearchPayloadString {
	std::string value;
	std::string dataType;
	std::string compareType;
};

enum DataType {
	INT_TYPE,
	FLOAT_TYPE,
	BOOL_TYPE
};

enum CompareType {
	EQUALS,
	INCREASED,
	DECREASED
};


template <class T>
struct ValueSearchPayloadTyped {
	T value;
	DataType dataType;
	CompareType compareType;
};

template <class T>
class Pointerscanner {
private:
	std::vector<MemoryEntry<T>> currentEntries;
	std::shared_ptr<EasyMem> memory;
	std::shared_ptr<PipeClient> pipeClient;
	std::shared_ptr<Module> module;
	DataType currentDataType;
public:
	std::vector<MemoryEntry<T>> searchInitialValue(ValueSearchPayloadTyped<T> payload);
	std::vector<MemoryEntry<T>> searchNextValue(ValueSearchPayloadTyped<T> payload);
	std::vector<MemoryEntry<T>> updateCurrentEntries();
	std::vector<PointerEntry<T>> updatePointers(std::vector <PointerEntry<T>> entries);
	std::vector<PointerEntry<T>> searchStaticPointers(uintptr_t address);
	Pointerscanner(std::shared_ptr<EasyMem> memory, std::shared_ptr<PipeClient> pipeClient, std::shared_ptr<Module> module, DataType dataType);
};

const uintptr_t start = 0x0000000000000000;
const uintptr_t end_64 = 0x00007FFFFFFFFFFF;
const uintptr_t end_32 = 0xffffffff;
const uintptr_t maxOffset = 2047;
const uintptr_t end = end_32;

template<class T>
std::string ToHex(const T &value)
{
	std::ostringstream oss;
	if (!(oss << std::hex << value)) return "failed";
	auto su = oss.str();
	transform(su.begin(), su.end(), su.begin(), ::toupper);
	return "0x" + su;
}


void TestWriteFilea(std::string a)
{
	std::ofstream myfile("test1337.txt", std::ios::out | std::ios::app);
	if (myfile.is_open())
	{
		myfile << a << std::endl;
	}
}


template <class T>
bool compareValues(T a, T b, CompareType compareType, DataType dataType) {
	auto compareTypeString = magic_enum::enum_name(compareType);
	auto dataTypeString = magic_enum::enum_name(dataType);

	if (compareTypeString == magic_enum::enum_name(CompareType::EQUALS)) {
		if (dataTypeString == magic_enum::enum_name(DataType::FLOAT_TYPE)) {
			return round(a) == round(b);
		}
		if (dataTypeString == magic_enum::enum_name(DataType::INT_TYPE)) {
			return a == b;
		}
		if (dataTypeString == magic_enum::enum_name(DataType::BOOL_TYPE)) {
			return a == b;
		}
	}
	if (compareTypeString == magic_enum::enum_name(CompareType::INCREASED)) {
		if (dataTypeString == magic_enum::enum_name(DataType::FLOAT_TYPE)) {
			return round(a) > round(b);
		}
		if (dataTypeString == magic_enum::enum_name(DataType::INT_TYPE)) {
			return a > b;
		}
		if (dataTypeString == magic_enum::enum_name(DataType::BOOL_TYPE)) {
			return a != b;
		}
	}
	if (compareTypeString == magic_enum::enum_name(CompareType::DECREASED)) {
		if (dataTypeString == magic_enum::enum_name(DataType::FLOAT_TYPE)) {
			return round(a) < round(b);
		}
		if (dataTypeString == magic_enum::enum_name(DataType::INT_TYPE)) {
			return a < b;
		}
		if (dataTypeString == magic_enum::enum_name(DataType::BOOL_TYPE)) {
			return a != b;
		}
	}
	return false;
}

template <class T>
auto scanMemory(HANDLE handle, ValueSearchPayloadTyped<T> payload) {
	auto addr_min = start;
	auto addr_max = end;

	auto found = 0;
	std::vector<MemoryEntry<T>> foundEntries = {};


	// Loop the pages of memory of the application..
	while (addr_min < addr_max)
	{
		MEMORY_BASIC_INFORMATION mbi = { 0 };
		if (!::VirtualQueryEx(handle, (LPCVOID)addr_min, &mbi, sizeof(mbi)))
		{
			break;
		}

		// Determine if we have access to the page..
		if (mbi.State == MEM_COMMIT && ((mbi.Protect & PAGE_GUARD) == 0) && ((mbi.Protect & PAGE_NOACCESS) == 0))
		{
			//
			// Below are flags about the current region of memory. If you want to specifically scan for only
			// certain things like if the area is writable, executable, etc. you can use these flags to prevent
			// reading non-desired protection types.
			//

			auto isCopyOnWrite = ((mbi.Protect & PAGE_WRITECOPY) != 0 || (mbi.Protect & PAGE_EXECUTE_WRITECOPY) != 0);
			auto isExecutable = ((mbi.Protect & PAGE_EXECUTE) != 0 || (mbi.Protect & PAGE_EXECUTE_READ) != 0 || (mbi.Protect & PAGE_EXECUTE_READWRITE) != 0 || (mbi.Protect & PAGE_EXECUTE_WRITECOPY) != 0);
			auto isWritable = ((mbi.Protect & PAGE_READWRITE) != 0 || (mbi.Protect & PAGE_WRITECOPY) != 0 || (mbi.Protect & PAGE_EXECUTE_READWRITE) != 0 || (mbi.Protect & PAGE_EXECUTE_WRITECOPY) != 0);

			if (isWritable) {

				// Dump the region into a memory block..
				auto dump = new unsigned char[mbi.RegionSize + 1];
				memset(dump, 0x00, mbi.RegionSize + 1);
				if (!::ReadProcessMemory(handle, mbi.BaseAddress, dump, mbi.RegionSize, NULL))
				{
					break;
				}

				// Scan for 4 byte value of 1337..
				for (auto x = 0; x < mbi.RegionSize - 4; x += 4)
				{
					auto pointer = (T*)(dump + x);
					if (compareValues<T>(*pointer, payload.value, payload.compareType, payload.dataType)) {
						foundEntries.push_back({ addr_min + x , 4, *pointer });
					}
				}

				// Cleanup the memory dump..
				delete[] dump;
			}
		}

		// Step the current address by this regions size..
		addr_min += mbi.RegionSize;
	}

	return foundEntries;
}

template <class T>
std::vector<PointerEntry<T>> getPointers(HANDLE handle, uintptr_t moduleBase, uintptr_t address) {
	TestWriteFilea("moduleBase: " + ToHex(moduleBase));
	auto addr_min = start;
	auto addr_max = end;

	auto found = 0;
	std::vector<PointerEntry<T>> foundEntries = {};

	// Loop the pages of memory of the application..
	while (addr_min < addr_max)
	{

		MEMORY_BASIC_INFORMATION mbi = { 0 };
		if (!::VirtualQueryEx(handle, (LPCVOID)addr_min, &mbi, sizeof(mbi)))
		{
			break;
		}

		// Determine if we have access to the page..
		if (mbi.State == MEM_COMMIT && ((mbi.Protect & PAGE_GUARD) == 0) && ((mbi.Protect & PAGE_NOACCESS) == 0))
		{
			//
			// Below are flags about the current region of memory. If you want to specifically scan for only
			// certain things like if the area is writable, executable, etc. you can use these flags to prevent
			// reading non-desired protection types.
			//

			auto isCopyOnWrite = ((mbi.Protect & PAGE_WRITECOPY) != 0 || (mbi.Protect & PAGE_EXECUTE_WRITECOPY) != 0);
			auto isExecutable = ((mbi.Protect & PAGE_EXECUTE) != 0 || (mbi.Protect & PAGE_EXECUTE_READ) != 0 || (mbi.Protect & PAGE_EXECUTE_READWRITE) != 0 || (mbi.Protect & PAGE_EXECUTE_WRITECOPY) != 0);
			auto isWritable = ((mbi.Protect & PAGE_READWRITE) != 0 || (mbi.Protect & PAGE_WRITECOPY) != 0 || (mbi.Protect & PAGE_EXECUTE_READWRITE) != 0 || (mbi.Protect & PAGE_EXECUTE_WRITECOPY) != 0);

			if (isWritable) {

				// Dump the region into a memory block..
				auto dump = new unsigned char[mbi.RegionSize + 1];
				memset(dump, 0x00, mbi.RegionSize + 1);
				if (!::ReadProcessMemory(handle, mbi.BaseAddress, dump, mbi.RegionSize, NULL))
				{
					break;
				}



				//TestWriteFilea("----AllocationBaseAddress---- " + ToHex(mbi.AllocationBase) + "----RegionSize---- " + ToHex(mbi.RegionSize));
				auto hasFound = false;
				for (auto x = 0; x < mbi.RegionSize - 4; x += 4)
				{

					if ((uintptr_t)address == addr_min + x) {
						hasFound = true;
						TestWriteFilea("found 1");
						break;
					}
				}

				const uintptr_t minDynamicAddress = address - maxOffset;
				if (hasFound) {
					for (auto x = 0; x < mbi.RegionSize - 4; x += 4)
					{
						auto pointerBase = (ptr_size*)(dump + x);
						if (*pointerBase >= minDynamicAddress && *pointerBase <= address) {

							TestWriteFilea("found " + ToHex(addr_min) + " " + ToHex(x) + " " + ToHex(address - *pointerBase));
							const auto offset = address - *pointerBase;
							const auto pointerBaseOffset = (addr_min + x) - moduleBase;

							foundEntries.push_back({ moduleBase, pointerBaseOffset, {offset}, address, 0 });
						}

					}
				}
				// Cleanup the memory dump..
				delete[] dump;
			}
		}

		// Step the current address by this regions size..
		addr_min += mbi.RegionSize;
	}
	return foundEntries;
}

template <class T>
Pointerscanner<T>::Pointerscanner(std::shared_ptr<EasyMem> memory, std::shared_ptr<PipeClient> pipeClient, std::shared_ptr<Module> module, DataType dataType)
{
	this->memory = memory;
	this->pipeClient = pipeClient;
	this->module = module;
	this->currentDataType = dataType;
}

template <typename T>
std::vector<MemoryEntry<T>> Pointerscanner<T>::searchInitialValue(ValueSearchPayloadTyped<T> payload) {
	this->currentEntries = scanMemory<T>(memory->getProcessHandle(), payload);

	return this->currentEntries;
}

template <typename T>
std::vector<MemoryEntry<T>> Pointerscanner<T>::searchNextValue(ValueSearchPayloadTyped<T> payload) {
	auto compareTypeString = magic_enum::enum_name(payload.compareType);
	std::vector<MemoryEntry<T>> foundEntries = {};
	auto nextValue = payload.value;

	for (auto entry : this->currentEntries) {
		const auto foundValue = memory->read<T>(entry.address);
		if (compareTypeString == magic_enum::enum_name(CompareType::INCREASED) || compareTypeString == magic_enum::enum_name(CompareType::DECREASED)) {
			if (compareValues<T>(foundValue, entry.value , payload.compareType, payload.dataType)) foundEntries.push_back({ entry.address, 4, foundValue });
		}
		else {
			if (compareValues<T>(foundValue, nextValue, payload.compareType, payload.dataType)) foundEntries.push_back({ entry.address, 4, foundValue });
		}
		
	}
	this->currentEntries = foundEntries;
	return foundEntries;
}

template <typename T>
std::vector<MemoryEntry<T>> Pointerscanner<T>::updateCurrentEntries()
{
	std::vector<MemoryEntry<T>> updatedEntries = {};
	for (auto entry : this->currentEntries) {
		auto value = this->memory->read<T>(entry.address);
		updatedEntries.push_back({ entry.address, 4, value });
	}
	return updatedEntries;
}

template <typename T>
std::vector<PointerEntry<T>> Pointerscanner<T>::updatePointers(std::vector<PointerEntry<T>> entries)
{
	std::vector<PointerEntry<T>> updatedEntries = {};
	for (auto entry : entries) {
		auto base = this->memory->read<uintptr_t>((uintptr_t)module->baseModuleRaw + entry.baseOffset);
		auto value = this->memory->read<T>(base + entry.offsets[0]);
		updatedEntries.push_back({ (uintptr_t)module->baseModuleRaw, entry.baseOffset, entry.offsets, base + entry.offsets[0], value });
	}
	return updatedEntries;
}

template <typename T>
std::vector<PointerEntry<T>> Pointerscanner<T>::searchStaticPointers(uintptr_t address) {
	return getPointers<T>(memory->getProcessHandle(), (uintptr_t)module->baseModuleRaw, address);
}


