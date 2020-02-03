import { MemoryEntry, PointerEntry } from "./PipeContext";

export type ValueSearchDataType = "INT_TYPE" | "FLOAT_TYPE" | "BOOL_TYPE";
export type ValueSearchCompareType = "EQUALS" | "DECREASED" | "INCREASED";
export interface ValueSearchSendPayload {
  value: number;
  dataType: ValueSearchDataType;
  compareType: ValueSearchCompareType;
}

export type SearchSendAction =
  | {
      type: "SEARCH_INITIAL_VALUE";
      payload: ValueSearchSendPayload;
    }
  | {
      type: "SEARCH_NEXT_VALUE";
      payload: ValueSearchSendPayload;
    }
  | {
      type: "UPDATE_ADDRESSES";
      payload: MemoryEntry[];
    }
  | {
      type: "UPDATE_POINTERS";
      payload: PointerEntry[];
    }
  | {
      type: "SEARCH_POINTERS";
      payload: number;
    };

export type SearchRecieveAction =
  | {
      type: "SEARCH_INITIAL_VALUE";
      payload: MemoryEntry[];
    }
  | { type: "SEARCH_NEXT_VALUE"; payload: MemoryEntry[] }
  | { type: "UPDATE_ADDRESSES"; payload: MemoryEntry[] }
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
  sendMessage: (message: SearchSendAction) => boolean
): SearchRecieveAction {
  const messageObj = {
    type: "SEARCH_INITIAL_VALUE" as const,
    payload: payload
  };

  sendMessage(messageObj);

  return {
    type: "UPDATE_LOADING_ADDRESSES"
  };
}

export function searchNextValue(
  payload: ValueSearchSendPayload,
  sendMessage: (message: SearchSendAction) => boolean
): SearchRecieveAction {
  const messageObj = {
    type: "SEARCH_NEXT_VALUE" as const,
    payload: payload
  };
  sendMessage(messageObj);

  return {
    type: "UPDATE_LOADING_ADDRESSES"
  };
}

export function updateAddresses(
  payload: MemoryEntry[],
  sendMessage: (message: SearchSendAction) => boolean
) {
  const messageObj = {
    type: "UPDATE_ADDRESSES" as const,
    payload: payload
  };
  sendMessage(messageObj);

}

export function searchPointers(
  address: number,
  sendMessage: (message: SearchSendAction) => boolean
): SearchRecieveAction {
  const messageObj = {
    type: "SEARCH_POINTERS" as const,
    payload: address
  };

  sendMessage(messageObj);
  return {
    type: "UPDATE_LOADING_POINTERS"
  };
}

export function updatePointers(
  pointers: PointerEntry[],
  sendMessage: (message: SearchSendAction) => boolean
): SearchRecieveAction {
  const messageObj = {
    type: "UPDATE_POINTERS" as const,
    payload: pointers
  };

  sendMessage(messageObj);
  return {
    type: "UPDATE_LOADING_POINTERS"
  };
}
