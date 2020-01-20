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
        pointers: PointerEntry[];
        shouldPointTo: number;
      };
    }
  | {
      type: "IMPORT_POINTERS";
      payload: {
        pointers: PointerEntry[];
        shouldPointTo: number;
      };
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
