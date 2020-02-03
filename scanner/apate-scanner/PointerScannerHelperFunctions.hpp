#pragma once
#include "Pointerscanner.hpp"

namespace PointerScannerHelperFunctions
{
	template <class T>
	std::vector<MemoryEntry<T>> getAddressesFromScanType(std::string type, std::shared_ptr<Pointerscanner<T>> pointerscanner, json payload) {
			ValueSearchPayloadTyped<T> searchPayloadTyped = payload;
			if (type == "SEARCH_INITIAL_VALUE") {
				return pointerscanner->searchInitialValue(searchPayloadTyped);
			}
			if (type == "SEARCH_NEXT_VALUE") {
				return pointerscanner->searchNextValue(searchPayloadTyped);
			}
		}

	/*template <class T> ADD FACTORY
	std::shared_ptr<Pointerscanner<T>> getPointerscannerFromDataType() {
		if (dataTypeString == magic_enum::enum_name(DataType::INT_TYPE)) {
			return pointerscannerInt;
		}
		else if (dataTypeString == magic_enum::enum_name(DataType::FLOAT_TYPE)) {
			return pointerscannerFloat;
		}
		else if (dataTypeString == magic_enum::enum_name(DataType::BOOL_TYPE)) {
			return pointerscannerBool;
		}
	}*/
};

