/* eslint-disable @typescript-eslint/no-var-requires */

/** @type {import('tailwindcss').Config} */
module.exports = {
  darkMode: 'class',
  content: ['./index.html', './src/**/*.{js,ts,jsx,tsx}'],
  theme: {
    extend: {
      colors: {
        blue: '#222F5B',
        cloud: '#F0F0F4',
        current: 'currentColor',
        gray: '#383936',
        green: '#80D18E',
        neutral: '#F0F3EE',
        sky: '#3C65F5',
        slate: '#161717',
        transparent: 'transparent',
        white: '#FFFFFF',
        zinc: '#8A8D86',
      },
    },
  },
  plugins: [require('tailwindcss-radix')()],
};
