#include "Pointerscanner.h"
#include <vector>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <memory>
#include <fstream>
#include <sstream>


const uintptr_t start = 0x0000000000000000;
const uintptr_t end_64 = 0x00007FFFFFFFFFFF;
const uintptr_t end_32 = 0xffffffff;
const uintptr_t maxOffset = 2047;
const uintptr_t end = end_64;

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

auto scanMemory(HANDLE handle, int value) {

	auto addr_min = start;
	auto addr_max = end;

	auto found = 0;
	std::vector<MemoryEntry> foundEntries = {};
	

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
					auto pointer = (ptr_size*)(dump + x);
					if (*pointer == value) {
						foundEntries.push_back({ addr_min + x , 4, value });
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

std::vector<PointerEntry> getPointers(HANDLE handle, uintptr_t moduleBase, uintptr_t address) {
	TestWriteFilea("moduleBase: " + ToHex(moduleBase));
	auto addr_min = start;
	auto addr_max = end;

	auto found = 0;
	std::vector<PointerEntry> foundEntries = {};

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
				for (auto x = 0; x < mbi.RegionSize - 4; x+= 4)
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
								
								TestWriteFilea("found " + ToHex(addr_min) + " " +  ToHex(x) + " " + ToHex(address - *pointerBase));
								const auto offset = address - *pointerBase;
								const auto pointerBaseOffset = (addr_min + x) - moduleBase;
								auto valuePointer = (ptr_size*)(*pointerBase + offset);
								foundEntries.push_back({ moduleBase, pointerBaseOffset, {offset}, address, (int)*valuePointer });
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

Pointerscanner::Pointerscanner(std::shared_ptr<EasyMem> memory, std::shared_ptr<PipeClient> pipeClient, std::shared_ptr<Module> module)
{
	this->memory = memory;
	this->pipeClient = pipeClient;
	this->module = module;
}

std::vector<MemoryEntry> Pointerscanner::searchInitialValue(int value) { 
	this->currentEntries = scanMemory(memory->getProcessHandle(), value);
	return this->currentEntries;
}
std::vector<MemoryEntry> Pointerscanner::searchNextValue(int nextValue) {
	std::vector<MemoryEntry> foundEntries = {};
	for (auto entry : this->currentEntries) {
		const auto foundValue = memory->read<int>(entry.address);
		if (foundValue == nextValue) foundEntries.push_back({ entry.address, 4, nextValue });
	}
	this->currentEntries = foundEntries;
	return foundEntries;
}

std::vector<MemoryEntry> Pointerscanner::updateCurrentEntries()
{
	std::vector<MemoryEntry> updatedEntries = {};
	for (auto entry : this->currentEntries) {
		auto value = this->memory->read<int>(entry.address);
		updatedEntries.push_back({ entry.address, 4, value });
	}
	return updatedEntries;
}

std::vector<PointerEntry> Pointerscanner::updatePointers(std::vector<PointerEntry> entries)
{
	std::vector<PointerEntry> updatedEntries = {};
	for (auto entry : entries) {
		auto base = this->memory->read<int>(entry.baseAddress + entry.baseOffset);
		auto value = this->memory->read<int>(base + entry.offsets[0]);
		updatedEntries.push_back({entry.baseAddress, entry.baseOffset, entry.offsets, base + entry.offsets[0], value });
	}
	return updatedEntries;
}


std::vector<PointerEntry> Pointerscanner::searchStaticPointers(uintptr_t address) {
	return getPointers(memory->getProcessHandle(), (uintptr_t)module->baseModuleRaw, address);
}