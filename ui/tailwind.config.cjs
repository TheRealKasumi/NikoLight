/* eslint-disable @typescript-eslint/no-var-requires */

/** @type {import('tailwindcss').Config} */
module.exports = {
  darkMode: 'class',
  content: ['./index.html', './src/**/*.{js,ts,jsx,tsx}'],
  theme: {
    extend: {
      colors: {
        transparent: 'transparent',
        current: 'currentColor',
        slate: '#161717',
        gray: '#383936',
        zinc: '#8A8D86',
        neutral: '#F0F3EE',
        green: '#80D18E',
        sky: '#3C65F5',
        blue: '#222F5B',
      },
    },
  },
  plugins: [require('tailwindcss-radix')()],
};
