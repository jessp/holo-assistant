import React from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import App from './App';
import reportWebVitals from './reportWebVitals';
import PiwikPro from '@piwikpro/react-piwik-pro';

PiwikPro.initialize('34c68ac1-60e3-479c-99fb-45fb49a2e649', 'https://awardwinninghuman.containers.piwik.pro');

const root = ReactDOM.createRoot(
  document.getElementById('root') as HTMLElement
);
root.render(
    <App />
);
