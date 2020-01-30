import { MemoryEntry, PointerEntry } from "./PipeContext";
import { SearchRecieveAction } from "./ValueSearchActionCreators";

export interface SearchState {
  currentAddresses: MemoryEntry[];
  currentSearchedValue: number | null;
  currentPointers: PointerEntry[];
  currentShouldPointTo: number | null;
  loading: {
    addresses: boolean;
    pointers: boolean;
  };
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
  currentShouldPointTo: null,
  loading: {
    addresses: false,
    pointers: false
  }
};

export const searchReducer = (
  state: SearchState,
  action: SearchRecieveAction
): SearchState => {
  switch (action.type) {
    case "SEARCH_INITIAL_VALUE":
      return {
        ...state,
        currentAddresses: action.payload,
        loading: {
          ...state.loading,
          addresses: false
        }
      };
    case "SEARCH_NEXT_VALUE":
      return {
        ...state,
        currentAddresses: action.payload,
        loading: {
          ...state.loading,
          addresses: false
        }
      };
    case "SEARCH_POINTERS":
      return {
        ...state,
        currentPointers: action.payload.pointerEntries,
        currentShouldPointTo: action.payload.shouldPointTo,
        loading: {
          ...state.loading,
          pointers: false
        }
      };

    case "UPDATE_POINTERS":
      return {
        ...state,
        currentPointers: action.payload.pointerEntries,
        currentShouldPointTo: action.payload.shouldPointTo,
        loading: {
          ...state.loading,
          pointers: false
        }
      };

    case "UPDATE_SHOULDPOINT":
      return {
        ...state,
        currentShouldPointTo: action.payload
      };
    case "UPDATE_LOADING_ADDRESSES":
      return {
        ...state,
        loading: {
          addresses: true,
          pointers: state.loading.pointers
        }
      };
    case "UPDATE_LOADING_POINTERS":
      return {
        ...state,
        loading: {
          addresses: state.loading.addresses,
          pointers: true
        }
      };

    default:
      return state;
  }
};
