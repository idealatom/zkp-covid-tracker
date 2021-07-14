import React from 'react';
import { ThemeProvider } from '@material-ui/styles';
import theme from '~/theme';
import useStyles from './styles';
import MainLayout from './components/MainLayout';
import ProofView from './components/ProofView';

function App() {
  useStyles();
  return (
    <ThemeProvider theme={theme}>
      <MainLayout>
        <ProofView />
      </MainLayout>
    </ThemeProvider>
  );
}

export default App;
