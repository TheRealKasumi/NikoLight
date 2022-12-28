import React from 'react';
import ReactDOM from 'react-dom/client';
import SwaggerUI from 'swagger-ui-react';
import 'swagger-ui-react/swagger-ui.css';
import spec from './pages/api/openapi.json';

ReactDOM.createRoot(document.getElementById('root') as HTMLElement).render(
  <React.StrictMode>
    <SwaggerUI spec={spec} />
  </React.StrictMode>,
);
