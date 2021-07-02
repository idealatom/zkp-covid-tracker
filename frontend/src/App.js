import "./App.css";
import { Box, Container, Typography } from "@material-ui/core";

function App() {
  return (
    <Container maxWidth="sm">
      <Box my={4}>
        <Typography variant="h4" component="h1" gutterBottom>
          Hello
        </Typography>
      </Box>
    </Container>
  );
}

export default App;
