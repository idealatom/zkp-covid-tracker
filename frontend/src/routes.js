import React from 'react';
import { Navigate } from 'react-router-dom';

import MainLayout from '~/components/MainLayout';
import ProofView from '~/components/ProofView';
import VerifyView from '~/components/VerifyView';

const routes = () => [
  {
    path: '/',
    element: <MainLayout />,
    children: [
      { path: '/', element: <Navigate to="/proof" replace /> },
      { path: 'proof', element: <ProofView /> },
      { path: 'verify', element: <VerifyView /> },
    ],
  },
];

export default routes;
