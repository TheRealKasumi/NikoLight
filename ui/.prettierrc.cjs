/** @type {import('@types/prettier').Options} */
module.exports = {
  singleQuote: true,
  trailingComma: 'all',
  plugins: [require('prettier-plugin-tailwindcss')],
};
