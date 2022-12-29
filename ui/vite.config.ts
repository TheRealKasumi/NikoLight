import react from '@vitejs/plugin-react';
import { resolve } from 'node:path';
import { defineConfig } from 'vite';
import svgr from 'vite-plugin-svgr';

// https://vitejs.dev/config/
export default defineConfig({
  base: '/ui/',
  plugins: [react(), svgr()],
  build: {
    rollupOptions: {
      input: {
        main: resolve(__dirname, 'index.html'),
        swagger: resolve(__dirname, 'swagger.html'),
      },
    },
  },
});
