import React from "react";
import { Box, Container, Typography } from "@material-ui/core";

function ProofView() {
  return (
    <Container maxWidth="sm">
      <Box my={4}>
        <Typography variant="h4" component="h1" gutterBottom>
          Proof
        </Typography>
      </Box>
    </Container>
  );
}

export default ProofView;
