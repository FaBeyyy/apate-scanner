#pragma once
#include "Pointerscanner.h"

namespace PointerScannerHelperFunctions
{
	template <class T>
	std::vector<MemoryEntry<T>> getAddressesFromScanType(std::string type, std::shared_ptr<Pointerscanner<T>> pointerscanner, json payload);
};

