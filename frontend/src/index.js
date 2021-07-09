import React from 'react';
import ReactDOM from 'react-dom';
import { BrowserRouter } from 'react-router-dom';
import './index.css';
import App from './App';
import { StoreProvider } from './Store';
import { initialState, proofListReducer } from './state';

ReactDOM.render(
  <React.StrictMode>
    <StoreProvider initialState={initialState} reducer={proofListReducer}>
      <BrowserRouter>
        <App />
      </BrowserRouter>
    </StoreProvider>
  </React.StrictMode>,
  document.getElementById('root')
);
