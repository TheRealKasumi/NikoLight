/**
 * @file LedService.js
 * @author TheRealKasumi
 * @brief Service for accessign the LED api of the TesLight controller.
 * @version 0.0.1
 * @date 2022-07-22
 *
 * @copyright Copyright (c) 2022
 *
 */
class LedService {
	/**
	 * Create a new instance of {@link LedService}.
	 * @param {string} basePath
	 */
	constructor(basePath) {
		this.basePath = basePath;
		this.colorSpaceConverter = new ColorSpaceConverter();
	}

	/**
	 * Load the LED configuration currently saved on the TesLight
	 * controller and decode it to be used by the frontend.
	 * @returns Promise with the converted data or {@link LedServiceException} when there was an error
	 */
	getLedConfiguration = () => {
		return new Promise(async (resolve, reject) => {
			let binConfig;
			try {
				binConfig = this.base64ToByteArray(await this.getTeslightLedConfiguration());
			} catch (ex) {
				reject(new LedServiceException("Failed to get LED configuration from the TesLight controller.", ex));
				return;
			}

			const config = [];
			for (let i = 0, j = 0; i < 6; i++) {
				let frontendConfig = {
					outputPin: binConfig[j++],
					ledCount: this.bytesToWord(binConfig[j++], binConfig[j++]),
					animatorType: binConfig[j++],
					speed: binConfig[j++],
					offset: this.bytesToWord(binConfig[j++], binConfig[j++]),
					brightness: binConfig[j++],
					reverse: binConfig[j++],
					hue1: 0,
					satturation1: 0,
					hue2: 0,
					saturation2: 0,
				};

				switch (frontendConfig.animatorType) {
					// Rainbow
					case 0:
					case 1:
						// Skip custom properties
						j += 10;
						break;

					// Gradient and static color
					case 2:
					case 3:
						const rgb1 = {
							red: binConfig[j++],
							green: binConfig[j++],
							blue: binConfig[j++],
						};
						const rgb2 = {
							red: binConfig[j++],
							green: binConfig[j++],
							blue: binConfig[j++],
						};
						const hsv1 = this.colorSpaceConverter.rgbToHsv(rgb1);
						const hsv2 = this.colorSpaceConverter.rgbToHsv(rgb2);
						frontendConfig.hue1 = hsv1.hue;
						frontendConfig.satturation1 = hsv1.satturation;
						frontendConfig.hue2 = hsv2.hue;
						frontendConfig.satturation2 = hsv2.satturation;

						// Skip remaining properties
						j += 4;

						break;
				}

				config.push(frontendConfig);
			}
			resolve(config);
		});
	};

	/**
	 * Encode and send the LED configuration of the frontend to the TesLight controller.
	 * @param {*} configuration configuration to be send
	 * @returns Promis with true when successful or the error otherwise
	 */
	postLedConfiguration = (configuration) => {
		return new Promise(async (resolve, reject) => {
			const binConfig = new Uint8Array(19 * configuration.length);
			for (let i = 0, j = 0; i < configuration.length; i++) {
				binConfig[j++] = configuration[i].outputPin;
				binConfig[j++] = this.wordToBytes(configuration[i].ledCount).first;
				binConfig[j++] = this.wordToBytes(configuration[i].ledCount).second;
				binConfig[j++] = configuration[i].animatorType;
				binConfig[j++] = configuration[i].speed;
				binConfig[j++] = this.wordToBytes(configuration[i].offset).first;
				binConfig[j++] = this.wordToBytes(configuration[i].offset).second;
				binConfig[j++] = configuration[i].brightness;
				binConfig[j++] = configuration[i].reverse;

				switch (configuration[i].animatorType) {
					// Rainbow
					case 0:
					case 1:
						// No custom properties
						j += 10;
						break;

					// Gradient and static color
					case 2:
					case 3:
						const hsv1 = {
							hue: configuration[i].hue1 / 255.0,
							satturation: configuration[i].satturation1 / 255.0,
							value: 1.0,
						};
						const hsv2 = {
							hue: configuration[i].hue2 / 255.0,
							satturation: configuration[i].satturation2 / 255.0,
							value: 1.0,
						};
						const rgb1 = this.colorSpaceConverter.hsvToRgb(hsv1);
						const rgb2 = this.colorSpaceConverter.hsvToRgb(hsv2);

						binConfig[j++] = rgb1.red;
						binConfig[j++] = rgb1.green;
						binConfig[j++] = rgb1.blue;
						binConfig[j++] = rgb2.red;
						binConfig[j++] = rgb2.green;
						binConfig[j++] = rgb2.blue;

						j += 4;
						break;
				}
			}

			const base64 = this.byteArrayToBase64(binConfig);

			try {
				resolve(this.postTeslightLedConfiguration(base64));
			} catch (ex) {
				reject(new LedServiceException("Failed to post LED configuration to the TesLight controller.", ex));
			}
		});
	};

	/**
	 * Query the encoded LED configuration from the TesLight controller.
	 * @returns Promise with the base64 encoded data or {@link LedServiceException} when the request fails
	 */
	getTeslightLedConfiguration = () => {
		return new Promise((resolve, reject) => {
			const url = this.basePath + "config/led";

			const options = {
				method: "GET",
				headers: {
					Accept: "applicatoin/json",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new LedServiceException(
							"The status code " + response.status + ' implies an error: "' + response.text() + '"'
						);
					}

					return response.text();
				})
				.then((data) => resolve(data))
				.catch((ex) => reject(ex));
		});
	};

	/**
	 * Post the encoded LED configuration to the TesLight controller.
	 * @param {*} ledConfiguration LED configuration
	 * @returns Promise containing true or {@link LedServiceException} when the request fails
	 */
	postTeslightLedConfiguration = (ledConfiguration) => {
		return new Promise((resolve, reject) => {
			const url = this.basePath + "config/led";

			const options = {
				method: "POST",
				headers: {
					"Content-Type": "application/x-www-form-urlencoded",
				},
				body: "data=" + encodeURIComponent(ledConfiguration),
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 202) {
						throw new LedServiceException(
							"The status code " + response.status + ' implies an error: "' + response.text() + '"'
						);
					}

					return true;
				})
				.then((data) => resolve(data))
				.catch((ex) => reject(ex));
		});
	};

	/**
	 * Convert a base64 string into a byte array.
	 * @param {string} base64String base64 encoded string
	 * @returns byte array
	 */
	base64ToByteArray = (base64String) => {
		const binaryString = window.atob(base64String);
		const length = binaryString.length;
		const bytes = new Uint8Array(length);
		for (let i = 0; i < length; i++) {
			bytes[i] = binaryString.charCodeAt(i);
		}
		return bytes;
	};

	/**
	 * Convert a byte array into a base64 encoded string.
	 * @param {Uint8Array} bytes
	 * @returns base64 encoded string
	 */
	byteArrayToBase64 = (bytes) => {
		let binary = "";
		let length = bytes.byteLength;
		for (let i = 0; i < length; i++) {
			binary += String.fromCharCode(bytes[i]);
		}
		return window.btoa(binary);
	};

	/**
	 * Convert two bytes into a word
	 * @param {*} first byte 1
	 * @param {*} second byte 2
	 * @returns word representation of the two bytes
	 */
	bytesToWord = (first, second) => {
		return ((second & 0xff) << 8) | (first & 0xff);
	};

	/**
	 * Convert a word into two bytes.
	 * @param {*} word input value
	 * @returns object with first and second byte
	 */
	wordToBytes = (word) => {
		return {
			first: word & 0xff,
			second: (word >> 8) & 0xff,
		};
	};
}
