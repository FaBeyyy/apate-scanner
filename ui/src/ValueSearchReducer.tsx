import { MemoryEntry, PointerEntry } from "./PipeContext";
import { SearchSendAction } from "./ValueSearchActionCreators";

export interface SearchState {
  currentAddresses: MemoryEntry[];
  currentSearchedValue: number | null;
  currentPointers: PointerEntry[];
  currentShouldPointTo: number | null;
}

export const initalSearchState: SearchState = {
  currentAddresses: [
    // { address: 849846454, sizeInBytes: 4, value: 100 },
    // { address: 843457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 },
    // { address: Math.random() * 943457454, sizeInBytes: 4, value: 100 }
  ],
  currentSearchedValue: null,
  currentPointers: [
    // {
    //   baseAddress: 8454,
    //   baseOffset: 984565,
    //   offsets: [336],
    //   pointsTo: 8464664,
    //   value: 100
    // }
  ],
  currentShouldPointTo: null
};

export const searchReducer = (
  state: SearchState,
  action: SearchSendAction
): SearchState => {
  switch (action.type) {
    case "SEARCH_INITIAL_VALUE":
      return {
        ...state,
        currentAddresses: action.payload
      };
    case "SEARCH_NEXT_VALUE":
      return {
        ...state,
        currentAddresses: action.payload
      };
    case "SEARCH_POINTERS":
      return {
        ...state,
        currentPointers: action.payload.pointerEntries,
        currentShouldPointTo: action.payload.shouldPointTo
      };

    case "UPDATE_POINTERS":
      return {
        ...state,
        currentPointers: action.payload.pointerEntries,
        currentShouldPointTo: action.payload.shouldPointTo
      };

    case "UPDATE_SHOULDPOINT":
      return {
        ...state,
        currentShouldPointTo: action.payload
      };

    default:
      return state;
  }
};
