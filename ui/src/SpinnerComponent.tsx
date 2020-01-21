import React from "react";
import styled, { keyframes } from "styled-components";
import { Ellipsis } from 'react-css-spinners'

interface Props {
  size?: number;
}

export function SpinnerComponent({size}: Props) {
  return (
  <Ellipsis size={size} />
  );
}
