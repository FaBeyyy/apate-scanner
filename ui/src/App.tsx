import React, { useEffect } from "react";
import * as net from "net";
import { ValueSearchComponent } from "./ValueSearchComponent";
import { Color } from "./Color";
import styled from "styled-components";

const App = () => {
  return (
    <WindowContainer>
      <ValueSearchComponent />
    </WindowContainer>
  );
};

export default App;

const WindowContainer = styled.div`
  height: 100%;
  width: 100%;
  background: ${Color.mainBackground};
  color: ${Color.fontMain};
  font-family: "Open Sans";
  font-size: 14px;
  font-weight: 400;
  
`;
