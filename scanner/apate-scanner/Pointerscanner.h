#pragma once
#include <vector>
#include "json.hpp"
#include "EasyMem.hpp"
#include "PipeClient.h"
#include "Module.h"

using json = nlohmann::json;
typedef uintptr_t ptr_size;  //OR DWORD IF 32-Bit

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

