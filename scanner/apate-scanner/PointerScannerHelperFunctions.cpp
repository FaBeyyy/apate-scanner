#include "PointerScannerHelperFunctions.h"

template <class T>
std::vector<MemoryEntry<T>> PointerScannerHelperFunctions::getAddressesFromScanType(std::string type, std::shared_ptr<Pointerscanner<T>> pointerscanner, json payload) {
	ValueSearchPayloadTyped<T> searchPayloadTyped = payload;
	if (type == "SEARCH_INITIAL_VALUE") {
		return pointerscanner->searchInitialValue(searchPayloadTyped);
	}
	if (type == "SEARCH_NEXT_VALUE") {
		return pointerscanner->searchNextValue(searchPayloadTyped);
	}
}
