import React from "react";
import { Box, Container, Typography } from "@material-ui/core";
import MapBox from "./MapBox";

function ProofView() {
  return (
    <Container maxWidth="sm">
      <Box my={4}>
        <MapBox />
      </Box>
    </Container>
  );
}

export default ProofView;
