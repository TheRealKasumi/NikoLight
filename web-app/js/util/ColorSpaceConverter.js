/**
 * @file ColorSpaceConverter.js
 * @author TheRealKasumi
 * @brief Utility to convert between the HSV and RGB color space.
 *
 * @copyright Copyright (c) 2022
 *
 */
class ColorSpaceConverter {
	constructor() {}

	/**
	 * Convert from the HSV to the RGB color space.
	 * @param {*} hsv HSV value
	 * @returns RGB representation of the color
	 */
	hsvToRgb = (hsv) => {
		const i = Math.floor(hsv.hue * 6);
		const f = hsv.hue * 6 - i;
		const p = hsv.value * (1 - hsv.satturation);
		const q = hsv.value * (1 - f * hsv.satturation);
		const t = hsv.value * (1 - (1 - f) * hsv.satturation);
		let red, green, blue;

		switch (i % 6) {
			case 0:
				(red = hsv.value), (green = t), (blue = p);
				break;
			case 1:
				(red = q), (green = hsv.value), (blue = p);
				break;
			case 2:
				(red = p), (green = hsv.value), (blue = t);
				break;
			case 3:
				(red = p), (green = q), (blue = hsv.value);
				break;
			case 4:
				(red = t), (green = p), (blue = hsv.value);
				break;
			case 5:
				(red = hsv.value), (green = p), (blue = q);
				break;
		}
		return {
			red: Math.round(red * 255),
			green: Math.round(green * 255),
			blue: Math.round(blue * 255),
		};
	};

	/**
	 * Convert from the RGB to the HSV color space.
	 * @param {*} rgb RGB value
	 * @returns HSV representation of the color
	 */
	rgbToHsv = (rgb) => {
		const max = Math.max(rgb.red, rgb.green, rgb.blue);
		const min = Math.min(rgb.red, rgb.green, rgb.blue);
		const diff = max - min;
		let hue;
		const satturation = max === 0 ? 0 : diff / max;
		const value = max / 255;

		switch (max) {
			case min:
				hue = 0;
				break;
			case rgb.red:
				hue = rgb.green - rgb.blue + diff * (rgb.green < rgb.blue ? 6 : 0);
				hue /= 6 * diff;
				break;
			case rgb.green:
				hue = rgb.blue - rgb.red + diff * 2;
				hue /= 6 * diff;
				break;
			case rgb.blue:
				hue = rgb.red - rgb.green + diff * 4;
				hue /= 6 * diff;
				break;
		}

		return {
			hue: hue,
			satturation: satturation,
			value: value,
		};
	};
}
