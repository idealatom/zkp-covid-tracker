import React from 'react';
import { Box, Container, Typography } from '@material-ui/core';

function VerifyView() {
  return (
    <Container maxWidth="sm">
      <Box my={4}>
        <Typography variant="h4" component="h1" gutterBottom>
          Verify
        </Typography>
      </Box>
    </Container>
  );
}

export default VerifyView;
