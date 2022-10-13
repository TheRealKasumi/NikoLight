/**
 * Utility to convert between different color formats.
 */
class ColorUtil {
	/**
	 * Convert the hex value from the color picker to RGB.
	 * @param {string} hex hex representation of teh color value
	 * @returns object contiaining the RGB parts
	 */
	static hexToRgb = (hex) => {
		const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
		return {
			r: parseInt(result[1], 16),
			g: parseInt(result[2], 16),
			b: parseInt(result[3], 16),
		};
	};

	/**
	 * Convert a RGB value to a hex value.
	 * @param {object} rgb object containing the RGB value
	 */
	static rgbToHex = (rgb) => {
		return "#" + ((1 << 24) + (rgb.r << 16) + (rgb.g << 8) + rgb.b).toString(16).slice(1);
	};
}

export default ColorUtil;
