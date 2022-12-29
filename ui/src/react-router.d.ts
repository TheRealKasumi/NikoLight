import '@tanstack/react-router';
import React from 'react';

declare module '@tanstack/react-router' {
  interface RouteMeta {
    icon?: React.ReactElement;
    isVisible?: boolean;
    name: React.ReactNode;
    zoneId?: number;
  }
}
