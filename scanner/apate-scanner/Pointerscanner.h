#pragma once
#include <vector>
#include "json.hpp"
#include "EasyMem.hpp"
#include "PipeClient.h"
#include "Module.h"

using json = nlohmann::json;
typedef uintptr_t ptr_size;  //OR DWORD IF 32-Bit

struct MemoryEntry {
	uintptr_t address;
	int sizeInBytes;
	int value;
};

struct PointerEntry {
	uintptr_t baseAddress;
	uintptr_t baseOffset;
	std::vector<uintptr_t>offsets;
	uintptr_t pointsTo;
	int value;
};


class Pointerscanner {
private:
	std::vector<MemoryEntry> currentEntries;
	std::shared_ptr<EasyMem> memory;
	std::shared_ptr<PipeClient> pipeClient;
	std::shared_ptr<Module> module;
public:
	Pointerscanner(std::shared_ptr<EasyMem> memory, std::shared_ptr<PipeClient> pipeClient, std::shared_ptr<Module> module);
	std::vector<MemoryEntry> searchInitialValue(int value);
	std::vector<MemoryEntry> searchNextValue(int nextValue);
	std::vector<MemoryEntry> updateCurrentEntries();
	std::vector<PointerEntry> updatePointers(std::vector <PointerEntry> entries);
	std::vector<PointerEntry> searchStaticPointers(uintptr_t address);
};