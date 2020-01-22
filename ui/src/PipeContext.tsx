import React, {
  useEffect,
  useState,
  useMemo,
  createContext,
  useContext,
  useReducer
} from "react";
import * as net from "net";
import {
  searchReducer,
  initalSearchState,
  SearchState
} from "./ValueSearchReducer";
import { SearchSendAction } from "./ValueSearchActionCreators";

interface PipeContext {
  sendMessage: (message: string) => boolean;
  lastMessage: PipeData | null;
  searchState: SearchState;
  serverState: ServerState;
  dispatch: React.Dispatch<SearchSendAction>;
}

export const useLatestMessage = () => {
  return useContext(Context).lastMessage;
};

export const useSendMessage = () => {
  return useContext(Context).sendMessage;
};

export const useSearchState = () => {
  return useContext(Context).searchState;
};

export const useDispatch = () => {
  return useContext(Context).dispatch;
};

const Context = createContext({} as PipeContext);

type ServerState = "CONNECTED" | "DISCONNECTED" | "IDLE";

export type PipeData = SearchSendAction;

export interface MemoryEntry {
  address: number;
  sizeInBytes: number;
  value: number;
}

export interface PointerEntry {
  baseAddress: number;
  baseOffset: number;
  offsets: number[];
  pointsTo: number;
  value: number;
}

export function Pipe(props: any) {
  const PIPE_NAME = "mypipe1337";
  const PIPE_PATH = "\\\\.\\pipe\\" + PIPE_NAME;

  const [pipeServer, setPipeServer] = useState<net.Server | null>(null);
  const [pipeStream, setPipeStream] = useState<net.Socket | null>(null);
  const [serverState, setServerState] = useState<ServerState>("IDLE");
  const [message, setMessage] = useState<PipeData | null>(null);
  const [searchState, dispatch] = useReducer(searchReducer, initalSearchState);

  const sendMessage = (message: string) => {
    if (!pipeStream || serverState !== "CONNECTED") return false;
    const nullterm = message + "\0";
    pipeStream.write(nullterm, (err => console.log(err + ' sent')));
    return true;
  };

  useEffect(() => {
    const server = net.createServer(stream => {
      setPipeStream(stream);
      stream.on("data", (buffer: Buffer) => {
        const message = JSON.parse(buffer.toString());
        if (message) {
          console.log(message);
          if (message.type && message.payload) {
            dispatch(message);
          }
        }
      });
    });

    server.on("close", () => {
      setServerState("DISCONNECTED");
    });

    server.on("error", () => {
      setServerState("DISCONNECTED");
    });

    server.listen(PIPE_PATH, () => {
      setPipeServer(server);
      setServerState("CONNECTED");
    });
  }, []);

  return (
    <Context.Provider
      value={{
        sendMessage: sendMessage,
        lastMessage: message,
        searchState: searchState,
        serverState: serverState,
        dispatch: dispatch
      }}
    >
      {props.children}
    </Context.Provider>
  );
}
