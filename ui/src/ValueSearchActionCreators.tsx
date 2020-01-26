import { MemoryEntry, PointerEntry } from "./PipeContext";

export type ValueSearchDataType = "INT" | "FLOAT" | "BOOL";
export type ValueSearchCompareType = "EQUALS" | "DECREASED" | "INCREASED";
export interface ValueSearchSendPayload {
  value: number;
  dataType: ValueSearchDataType;
  compareType: ValueSearchCompareType;
}

export type SearchSendAction =
  | {
      type: "SEARCH_INITIAL_VALUE";
      payload: MemoryEntry[];
    }
  | { type: "SEARCH_NEXT_VALUE"; payload: MemoryEntry[] }
  | {
      type: "SEARCH_POINTERS";
      payload: {
        pointerEntries: PointerEntry[];
        shouldPointTo: number;
      };
    }
  | {
      type: "UPDATE_POINTERS";
      payload: {
        pointerEntries: PointerEntry[];
        shouldPointTo: number;
      };
    }
  | {
      type: "UPDATE_SHOULDPOINT";
      payload: number;
    }
  | {
      type: "UPDATE_LOADING_ADDRESSES";
    }
  | {
      type: "UPDATE_LOADING_POINTERS";
    };

export function searchInitialValue(
  payload: ValueSearchSendPayload,
  sendMessage: (message: string) => boolean
): SearchSendAction {
  const messageObj = {
    type: "SEARCH_INITIAL_VALUE",
    payload: payload
  };

  sendMessage(JSON.stringify(messageObj));

  return {
    type: "UPDATE_LOADING_ADDRESSES"
  };
}

export function searchNextValue(
  payload: ValueSearchSendPayload,

  sendMessage: (message: string) => boolean
): SearchSendAction {
  const messageObj = {
    type: "SEARCH_NEXT_VALUE",
    payload: payload
  };
  sendMessage(JSON.stringify(messageObj));

  return {
    type: "UPDATE_LOADING_ADDRESSES"
  };
}

export function searchPointers(
  address: number,
  sendMessage: (message: string) => boolean
): SearchSendAction {
  const messageObj = {
    type: "SEARCH_POINTERS",
    payload: address
  };

  sendMessage(JSON.stringify(messageObj));
  return {
    type: "UPDATE_LOADING_POINTERS"
  };
}

export function updatePointers(
  pointers: PointerEntry[],
  sendMessage: (message: string) => boolean
): SearchSendAction {
  const messageObj = {
    type: "UPDATE_POINTERS",
    payload: pointers
  };

  sendMessage(JSON.stringify(messageObj));
  return {
    type: "UPDATE_LOADING_POINTERS"
  };
}
