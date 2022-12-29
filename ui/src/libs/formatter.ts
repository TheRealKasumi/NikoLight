export const toPercentage = (value: number): number =>
  Math.ceil((value / 255) * 100);

export const toMbyte = (bytes: number): string =>
  (bytes / 1024.0 / 1024.0).toFixed(3);
