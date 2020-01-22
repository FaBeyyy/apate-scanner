import { MemoryEntry, PointerEntry } from "./PipeContext";

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
    };

export function searchInitialValue(
  value: number,
  sendMessage: (message: string) => boolean
) {
  const messageObj = {
    type: "SEARCH_INITIAL_VALUE",
    payload: value
  };

  sendMessage(JSON.stringify(messageObj));
}

export function searchNextValue(
  value: number,
  sendMessage: (message: string) => boolean
) {
  const messageObj = {
    type: "SEARCH_NEXT_VALUE",
    payload: value
  };
  sendMessage(JSON.stringify(messageObj));
}

export function searchPointers(
  address: number,
  sendMessage: (message: string) => boolean
) {
  const messageObj = {
    type: "SEARCH_POINTERS",
    payload: address
  };

  sendMessage(JSON.stringify(messageObj));
}

export function updatePointers(
  pointers: PointerEntry[],
  sendMessage: (message: string) => boolean
) {
  const messageObj = {
    type: "UPDATE_POINTERS",
    payload: pointers
  };

  sendMessage(JSON.stringify(messageObj));
}
