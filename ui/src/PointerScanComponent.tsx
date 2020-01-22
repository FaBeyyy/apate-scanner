import styled from "styled-components";
import { PointerEntry, useDispatch, useSendMessage } from "./PipeContext";
import React from "react";
import { toHex } from "./helper";
import { Color } from "./Color";
import { promises } from "fs";
import "@fortawesome/fontawesome-free/css/all.css";
import { SpinnerComponent } from "./SpinnerComponent";
import { updatePointers } from "./ValueSearchActionCreators";
const { dialog } = require("electron").remote;
const fs = promises;

interface Props {
  pointerEntries: PointerEntry[];
  shouldPointTo: number | null;
  isLoading: boolean;
}

export function PointerScanComponent({
  pointerEntries,
  shouldPointTo,
  isLoading
}: Props) {
  const dispatch = useDispatch();
  const sendMessage = useSendMessage();

  const onUpdatePointersClick = () => {
    if (pointerEntries.length < 1) return;
    updatePointers(pointerEntries, sendMessage);
  }

  const onSavePointersClick = () => {
    dialog
      .showSaveDialog({ filters: [{ name: "JSON", extensions: ["json"] }] })
      .then(res => {
        if (res.canceled) return;
        fs.writeFile(
          `${res.filePath}`,
          JSON.stringify({
            shouldPointTo: shouldPointTo,
            pointerEntries: pointerEntries
          }),
          "utf8"
        ).catch(err => alert(err));
      })
      .catch(err => err);
  };

  const onImportPointersClick = () => {
    dialog
      .showOpenDialog({ filters: [{ name: "JSON", extensions: ["json"] }] })
      .then(res => {
        if (res.canceled) return;
        fs.readFile(res.filePaths[0])
          .then(res => {
            const readFileObject = JSON.parse(res.toString());
            if (readFileObject.pointerEntries && readFileObject.shouldPointTo !== undefined) {
              dispatch({ type: "UPDATE_POINTERS", payload: readFileObject });
            }
          })
          .catch(err => alert(err));
      });
  };

  const renderPointerEntries = () => {
    if (isLoading)
      return (
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
      );
    return pointerEntries.map((entries, i) => (
      <ListItemContainer>
        <AddressItem>{toHex(entries.baseAddress)}</AddressItem>
        <AddressItem>{toHex(entries.baseOffset)}</AddressItem>
        <AddressItem>{toHex(entries.offsets[0])}</AddressItem>
        <AddressItem>{entries.value}</AddressItem>
        <AddressItem>{toHex(entries.pointsTo)}</AddressItem>
      </ListItemContainer>
    ));
  };
  return (
    <Container>
      <div
        style={{
          display: "flex",
          flexDirection: "row",
          justifyContent: "space-between"
        }}
      >
        {shouldPointTo ? (
          <div>
            Found {pointerEntries.length}{" "}
            {`pointer${pointerEntries.length > 1 ? "s" : ""}`} for{" "}
            {toHex(shouldPointTo)}
          </div>
        ) : (
          <div> </div>
        )}

        <div>
          <IconButtonContainer onClick={onImportPointersClick}>
            <i className="fas fa-download"></i>
          </IconButtonContainer>
          <IconButtonContainer onClick={onSavePointersClick}>
            <i className="fas fa-upload"></i>
          </IconButtonContainer>
          <IconButtonContainer onClick={onUpdatePointersClick}>
            <i className="fas fa-sync-alt"></i>
          </IconButtonContainer>
        </div>
      </div>
      <ListItemContainer style={{ marginTop: "5px" }}>
        <AddressItem>Base Address</AddressItem>
        <AddressItem>Base Offset</AddressItem>
        <AddressItem>Offset</AddressItem>
        <AddressItem>Value</AddressItem>
        <AddressItem>Points to</AddressItem>
      </ListItemContainer>
      <HeaderSeperator />
      {renderPointerEntries()}
    </Container>
  );
}

const Container = styled.div`
  padding: 10px;
  background: ${Color.secondaryBackground};
  border-radius: 5px;
  margin-top: 5px;
`;

const ListItemContainer = styled.div`
  display: flex;
`;

const AddressItem = styled.div`
  flex: 1;
`;
const HeaderSeperator = styled.div`
  height: 1px;
  background: rgba(255, 255, 255, 0.2);
  margin-top: 3px;
  margin-bottom: 3px;
`;

const IconButtonContainer = styled.div`
  cursor: pointer;
  display: inline-flex;
  :not(:first-of-type) {
    margin-left: 10px;
  }
`;
