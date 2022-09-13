/**
 * @file SystemService.js
 * @author TheRealKasumi
 * @brief Service for accessing the system settings api of the TesLight controller.
 *
 * @copyright Copyright (c) 2022
 *
 */
class SystemService {
	/**
	 * Create a new instance of {@link SystemService}.
	 * @param {string} basePath
	 */
	constructor(basePath) {
		this.basePath = basePath;
	}

	/**
	 * Load the system configuration currently saved on the TesLight
	 * controller and decode it to be used by the frontend.
	 * @returns Promise with the converted data or {@link SystemServiceException} when there was an error
	 */
	getSystemConfiguration = () => {
		return new Promise(async (resolve, reject) => {
			let binConfig;
			try {
				binConfig = this.base64ToByteArray(await this.getTeslightSystemConfiguration());
			} catch (ex) {
				reject(new LedServiceException("Failed to get system configuration from the TesLight controller.", ex));
				return;
			}

			let index = 0;
			resolve({
				logLevel: binConfig[index++],
				lightSensorMode: binConfig[index++],
				lightSensorThreshold: this.bytesToWord(binConfig[index++], binConfig[index++]),
				lightSensorMinValue: this.bytesToWord(binConfig[index++], binConfig[index++]),
				lightSensorMaxValue: this.bytesToWord(binConfig[index++], binConfig[index++]),
			});
		});
	};

	/**
	 * Encode and send the system configuration of the frontend to the TesLight controller.
	 * @param {*} configuration configuration to be send
	 * @returns Promis with true when successful or the error otherwise
	 */
	postSystemConfiguration = (configuration) => {
		return new Promise(async (resolve, reject) => {
			const binConfig = new Uint8Array(8);

			let index = 0;
			binConfig[index++] = configuration.logLevel;
			binConfig[index++] = configuration.lightSensorMode;
			binConfig[index++] = this.wordToBytes(configuration.lightSensorThreshold).first;
			binConfig[index++] = this.wordToBytes(configuration.lightSensorThreshold).second;
			binConfig[index++] = this.wordToBytes(configuration.lightSensorMinValue).first;
			binConfig[index++] = this.wordToBytes(configuration.lightSensorMinValue).second;
			binConfig[index++] = this.wordToBytes(configuration.lightSensorMaxValue).first;
			binConfig[index++] = this.wordToBytes(configuration.lightSensorMaxValue).second;

			const base64 = this.byteArrayToBase64(binConfig);
			try {
				resolve(this.postTeslightSystemConfiguration(base64));
			} catch (ex) {
				reject(new SystemServiceException("Failed to post system configuration to the TesLight controller.", ex));
			}
		});
	};

	/**
	 * Query the encoded system configuration from the TesLight controller.
	 * @returns Promise with the base64 encoded data or {@link SystemServiceException} when the request fails
	 */
	getTeslightSystemConfiguration = () => {
		return new Promise((resolve, reject) => {
			const url = this.basePath + "config/system";

			const options = {
				method: "GET",
				headers: {
					Accept: "octet-stream",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new SystemServiceException(
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
	 * Post the encoded system configuration to the TesLight controller.
	 * @param {*} systemConfig system configuration
	 * @returns Promise containing true or {@link SystemServiceException} when the request fails
	 */
	postTeslightSystemConfiguration = (systemConfig) => {
		return new Promise((resolve, reject) => {
			const url = this.basePath + "config/system";

			const options = {
				method: "POST",
				headers: {
					"Content-Type": "application/x-www-form-urlencoded",
				},
				body: "data=" + encodeURIComponent(systemConfig),
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 202) {
						throw new WiFiServiceException(
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
