import React, { useState, useMemo, useEffect, useRef } from "react";
import styled, { css } from "styled-components";
import {
  useSearchState,
  useSendMessage,
  MemoryEntry,
  useDispatch
} from "./PipeContext";
import {
  searchInitialValue,
  searchNextValue,
  searchPointers,
  ValueSearchDataType,
  ValueSearchCompareType
} from "./ValueSearchActionCreators";
import { Color } from "./Color";
import {
  ContextMenu,
  MenuItem,
  ContextMenuTrigger,
  showMenu
} from "react-contextmenu";
import "./contextMenu.css";
import { PointerScanComponent } from "./PointerScanComponent";
import { toHex } from "./helper";
import { FixedSizeList as List } from "react-window";
import { AutoSizer } from "react-virtualized";
import { SpinnerComponent } from "./SpinnerComponent";

type SearchComponentState =
  | "IDLE"
  | "SEARCHED_INITAL"
  | "SEARCHED_NEXT"
  | "SEARCHED_POINTER"
  | "UPDATED";

export function ValueSearchComponent() {
  const [value, setValue] = useState<number | undefined>(undefined);
  const [dataType, setDataType] = useState<ValueSearchDataType>("INT");
  const [compareType, setCompareType] = useState<ValueSearchCompareType>(
    "EQUALS"
  );
  const [selectedAddress, setSelectedAddress] = useState<
    MemoryEntry | undefined
  >(undefined);

  const [savedAddresses, setSavedAddresses] = useState<
    Map<number, MemoryEntry>
  >(new Map([]));
  const [searchedAddresses, setSearchedAddresses] = useState<
    Map<number, MemoryEntry>
  >(new Map([]));
  const [componentState, setComponentState] = useState<SearchComponentState>(
    "IDLE"
  );

  const searchState = useSearchState();

  const sendMessage = useSendMessage();
  const dispatch = useDispatch();

  const isLoading = useMemo(() => {
    if (
      componentState === "SEARCHED_INITAL" ||
      componentState === "SEARCHED_NEXT"
    )
      return true;
    return false;
  }, [componentState]);

  useEffect(() => {
    const newSearchedAddresses = new Map(
      searchState.currentAddresses.map(entry => [entry.address, entry])
    );
    if (searchedAddresses.size === 0 || componentState === "SEARCHED_NEXT") {
      setSearchedAddresses(newSearchedAddresses);
    } else {
      newSearchedAddresses.forEach((value, key, map) => {
        if (savedAddresses.has(key)) {
          savedAddresses.set(key, value);
          map.delete(key);
        }
      });
      setSearchedAddresses(newSearchedAddresses);
    }
  }, [searchState.currentAddresses]);

  useEffect(() => {
    setComponentState("UPDATED");
  }, [searchState.currentAddresses, searchState.currentPointers]);

  const onValueChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    if (event.target.value == "") setValue(undefined);
    else setValue(Number(event.target.value));
  };

  const onSearchValueClick = () => {
    if (value == null) return;
    const payload = {
      value: value,
      compareType: compareType,
      dataType: dataType
    };
    dispatch(searchInitialValue(payload, sendMessage));
    setComponentState("SEARCHED_INITAL");
  };

  const onSearchNextValueClick = () => {
    if (value == null) return;
    const payload = {
      value: value,
      compareType: compareType,
      dataType: dataType
    };
    dispatch(searchNextValue(payload, sendMessage));
    setComponentState("SEARCHED_NEXT");
  };

  const renderAddressComponent = (memoryEntry: MemoryEntry) => (
    <AddressContainer
      selected={selectedAddress?.address === memoryEntry.address}
      onClick={() => setSelectedAddress(memoryEntry)}
      onMouseDown={() => setSelectedAddress(memoryEntry)}
    >
      <div style={{ flex: 1 }}>{toHex(memoryEntry.address)}</div>
      <div style={{ flex: 1 }}>{memoryEntry.value}</div>
    </AddressContainer>
  );

  const renderSearchState = () => (
    <>
      <ContextMenuTrigger id="searchedAddresses">
        {isLoading ? (
          <div
            style={{
              display: "flex",
              alignItems: "center",
              justifyContent: "center",
              marginTop: ""
            }}
          >
            <SpinnerComponent size={40} />
          </div>
        ) : (
          <>{renderWindowedList(Array.from(searchedAddresses))}</>
        )}
      </ContextMenuTrigger>

      <SearchedAddressesContextMenu />
    </>
  );

  const renderSavedState = () => (
    <>
      <ContextMenuTrigger id="savedAddresses">
        {renderWindowedList(Array.from(savedAddresses))}
      </ContextMenuTrigger>
      <SavedAddressesContextMenu />
    </>
  );

  const renderRow = useMemo(
    () => (array: Array<any>) => ({
      index,
      style
    }: {
      index: number;
      style: React.CSSProperties;
    }) => {
      const [address, memoryEntry] = array[index];
      return <div style={style}>{renderAddressComponent(memoryEntry)}</div>;
    },
    [renderAddressComponent]
  );

  const renderWindowedList = useMemo(
    () => (map: Array<any>) => (
      <div style={{ height: "300px" }}>
        <AutoSizer>
          {({ height, width }) => (
            <AddressesContainer
              height={height}
              itemCount={map.length}
              itemSize={19}
              width={width}
            >
              {renderRow(map)}
            </AddressesContainer>
          )}
        </AutoSizer>
      </div>
    ),
    [renderRow]
  );

  const onAddToSavedListClick = () => {
    const entry = selectedAddress;
    if (!entry) return;
    const newSearchedAddresses = new Map(searchedAddresses);
    newSearchedAddresses.delete(entry.address);
    setSearchedAddresses(newSearchedAddresses);
    const newSavedAddresses = new Map(savedAddresses);
    newSavedAddresses.set(entry.address, entry);
    setSavedAddresses(newSavedAddresses);
  };

  const onPointerScanClick = () => {
    if (!selectedAddress) return;
    dispatch({
      type: "UPDATE_SHOULDPOINT",
      payload: selectedAddress.address
    });
    dispatch(searchPointers(selectedAddress.address, sendMessage));
    setComponentState("SEARCHED_POINTER");
  };

  const onRemoveAddressClick = () => {
    if (!selectedAddress) return;
    const newSavedAddresses = new Map(savedAddresses);
    newSavedAddresses.delete(selectedAddress.address);
    setSavedAddresses(newSavedAddresses);
  };

  const SearchedAddressesContextMenu = useMemo(
    () => () => (
      <ContextMenu id="searchedAddresses">
        <MenuItem onClick={onAddToSavedListClick}>Add to saved list</MenuItem>
      </ContextMenu>
    ),
    [onAddToSavedListClick]
  );

  const SavedAddressesContextMenu = useMemo(
    () => () => (
      <ContextMenu id="savedAddresses">
        <MenuItem onClick={onPointerScanClick}>Pointer scan</MenuItem>
        <MenuItem divider={true} />
        <MenuItem onClick={onRemoveAddressClick}>Remove</MenuItem>
      </ContextMenu>
    ),
    [onPointerScanClick, onRemoveAddressClick]
  );

  const onValueSearchDataTypeChange = (
    event: React.ChangeEvent<HTMLSelectElement>
  ) => {
    const value = event.target.value as ValueSearchDataType;
    setDataType(value);
  };

  const onValueSearchCompareTypeChange = (
    event: React.ChangeEvent<HTMLSelectElement>
  ) => {
    const value = event.target.value as ValueSearchCompareType;
    setCompareType(value);
  };

  return (
    <Container>
      <ContentContainer>
        <div style={{ display: "flex", flexDirection: "row" }}>
          <SearchValueButton onClick={onSearchValueClick}>
            First scan
          </SearchValueButton>

          <ValueInput type="number" value={value} onChange={onValueChange} />

          <SearchValueButton onClick={onSearchNextValueClick}>
            Next scan
          </SearchValueButton>

          <SelectInput onChange={onValueSearchDataTypeChange}>
            <SelectOption value={"INT" } selected={dataType === "INT"}>
              int
            </SelectOption>
            <SelectOption
              value={"FLOAT" }
              selected={dataType === "FLOAT"}
            >
              float
            </SelectOption>
            <SelectOption
              value={"BOOL" }
              selected={dataType === "BOOL"}
            >
              bool
            </SelectOption>
          </SelectInput>
          <SelectInput onChange={onValueSearchCompareTypeChange}>
            <SelectOption
              value={"EQUALS" }
              selected={compareType === "EQUALS"}
            >
              Equals
            </SelectOption>
            <SelectOption
              value={"INCREASED" }
              selected={compareType === "INCREASED"}
            >
              Increased
            </SelectOption>
            <SelectOption
              value={"DECREASED" }
              selected={compareType === "DECREASED"}
            >
              Decreased
            </SelectOption>
          </SelectInput>
        </div>

        <div style={{ padding: "5px" }}>
          <div>
            Found:{" "}
            {value !== undefined && componentState !== "IDLE"
              ? searchState.currentAddresses.length
              : ""}
          </div>
        </div>

        <ContainerAddresses>
          <InnerContainer>
            <AddressesHeader>
              <AddressesHeaderItem>Address</AddressesHeaderItem>
              <AddressesHeaderItem>Value</AddressesHeaderItem>
            </AddressesHeader>
            <HeaderSeperator></HeaderSeperator>
            {renderSearchState()}
          </InnerContainer>

          <ColSeperator></ColSeperator>
          <InnerContainer>
            <AddressesHeader>
              <AddressesHeaderItem>Address</AddressesHeaderItem>
              <AddressesHeaderItem>Value</AddressesHeaderItem>
            </AddressesHeader>
            <HeaderSeperator></HeaderSeperator>
            {renderSavedState()}
          </InnerContainer>
        </ContainerAddresses>
      </ContentContainer>
      <PointerScanComponent
        pointerEntries={searchState.currentPointers}
        shouldPointTo={searchState.currentShouldPointTo}
        isLoading={searchState.loading.pointers}
      />
    </Container>
  );
}

const Container = styled.div``;

const ContentContainer = styled.div`
  background: ${Color.secondaryBackground};
  border-radius: 5px;
  padding: 10px;
`;

const ContainerAddresses = styled.div`
  display: flex;
`;

const InnerContainer = styled.div`
  width: 100%;
`;

const InputContainer = styled.div`
  background: ${Color.fourthBackground};
  height: fit-content;
  width: fit-content;
  display: inline-block;
  border-radius: 0px;
`;

const ValueInput = styled.input`
  background: ${Color.fourthBackground};
  height: 30px;
  box-sizing: border-box;
  padding: 5px;
  border-radius: 0px;
  color: #fff;
  border: 2px solid transparent;
  :focus {
    outline: 0px !important;
    border-image: linear-gradient(
        135deg,
        rgba(156, 32, 170, 1) 0%,
        rgba(251, 53, 112, 1) 100%
      )
      1;
  }
  ::-webkit-inner-spin-button {
    -webkit-appearance: none;
    margin: 0;
  }
`;

const AddressesHeader = styled.div`
  display: flex;
  flex-direction: "row";
  margin-right: 4px;
`;

const AddressesHeaderItem = styled.div`
  flex: 1;
  padding-left: 5px;
`;

const AddressesContainer = styled(List)`
  overflow-y: scroll !important;
  /* width */
  ::-webkit-scrollbar {
    width: 4px;
  }

  /* Track */
  ::-webkit-scrollbar-track {
    box-shadow: inset 0 0 5px ${Color.fifthBackground};
    border-radius: 10px;
  }

  /* Handle */
  ::-webkit-scrollbar-thumb {
    background: linear-gradient(
      135deg,
      rgba(156, 32, 170, 1) 0%,
      rgba(251, 53, 112, 1) 100%
    );
    border-radius: 10px;
  }
`;
const AddressContainer = styled.div<{ selected: boolean }>`
  display: flex;
  flex-direction: row;
  cursor: pointer;

  border-radius: 2px;
  ${props =>
    props.selected
      ? `background: linear-gradient(
        135deg,
        rgba(156, 32, 170, 1) 0%,
        rgba(251, 53, 112, 1) 100%
      )
      `
      : ""};
  padding-left: 5px;
`;
const SearchValueButton = styled.button`
  background: ${Color.fifthBackground};
  border: 2px solid transparent;
  box-sizing: border-box;
  color: inherit;
  font-size: inherit;
  border-radius: 250px;
  height: 30px;
  padding: 5px;
  padding-left: 10px;
  padding-right: 10px;
  cursor: pointer;
  :last-of-type {
    border-top-left-radius: 0px;
    border-bottom-left-radius: 0px;
  }
  :first-of-type {
    border-top-right-radius: 0px;
    border-bottom-right-radius: 0px;
  }
  :focus {
    outline: none;
  }
`;

const HeaderSeperator = styled.div`
  height: 1px;
  background: rgba(255, 255, 255, 0.2);
  margin-top: 3px;
  margin-bottom: 3px;
`;

const ColSeperator = styled.div`
  width: 1px;
  background: rgba(255, 255, 255, 0.2);

  margin-right: 5px;
  margin-left: 5px;
`;

const SelectInput = styled.select`
  background: ${Color.fourthBackground};
  height: 30px;
  box-sizing: border-box;
  padding: 5px;
  border-radius: 2px;
  color: #fff;
  border: 2px solid transparent;
  margin-left: 5px;
  :focus {
    outline: 0px !important;
  }
  outline: 0px !important;
`;

const SelectOption = styled.option``;
