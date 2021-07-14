import React from 'react';
import { ThemeProvider } from '@material-ui/styles';
import theme from '~/theme';
import useStyles from './styles';
import ProofView from './components/ProofView';

function App() {
  useStyles();
  return (
    <ThemeProvider theme={theme}>
      <ProofView />
    </ThemeProvider>
  );
}

export default App;
