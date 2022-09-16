/**
 * @file WiFiService.js
 * @author TheRealKasumi
 * @brief Service for accessign the WiFi api of the TesLight controller.
 *
 * @copyright Copyright (c) 2022
 *
 */
class WiFiService {
	/**
	 * Create a new instance of {@link WiFiService}.
	 * @param {string} basePath
	 */
	constructor(basePath) {
		this.basePath = basePath;
	}

	/**
	 * Load the WiFi configuration currently saved on the TesLight
	 * controller and decode it to be used by the frontend.
	 * @returns Promise with the converted data or {@link WiFiServiceException} when there was an error
	 */
	getWiFiConfiguration = () => {
		return new Promise(async (resolve, reject) => {
			let binConfig = {
				data: null,
			};
			try {
				binConfig.data = this.base64ToByteArray(await this.getTeslightWiFiConfiguration());
			} catch (ex) {
				reject(new WiFiServiceException("Failed to get WiFi configuration from the TesLight controller.", ex));
				return;
			}

			resolve({
				accessPointSsid: this.readString(binConfig),
				accessPointPassword: this.readString(binConfig),
				accessPointChannel: this.readByte(binConfig),
				accessPointHidden: this.readBoolean(binConfig),
				accessPointMaxConnections: this.readByte(binConfig),
				wifiSsid: this.readString(binConfig),
				wifiPassword: this.readString(binConfig),
			});
		});
	};

	/**
	 * Encode and send the WiFi configuration of the frontend to the TesLight controller.
	 * @param {*} configuration configuration to be send
	 * @returns Promis with true when successful or the error otherwise
	 */
	postWiFiConfiguration = (configuration) => {
		return new Promise(async (resolve, reject) => {
			let bufSize = configuration.accessPointSsid.length + 2;
			bufSize += configuration.accessPointPassword.length + 2;
			bufSize += 3; // Channel, hidden, maxConnections
			bufSize += configuration.wifiSsid.length + 2;
			bufSize += configuration.wifiPassword.length + 2;

			const binConfig = {
				data: new Uint8Array(bufSize),
			};

			let index = 0;
			index = this.writeString(binConfig, index, configuration.accessPointSsid);
			index = this.writeString(binConfig, index, configuration.accessPointPassword);
			index = this.writeByte(binConfig, index, configuration.accessPointChannel);
			index = this.writeBoolean(binConfig, index, configuration.accessPointHidden);
			index = this.writeByte(binConfig, index, configuration.accessPointMaxConnections);
			index = this.writeString(binConfig, index, configuration.wifiSsid);
			index = this.writeString(binConfig, index, configuration.wifiPassword);

			const base64 = this.byteArrayToBase64(binConfig.data);

			try {
				resolve(this.postTeslightWiFiConfiguration(base64));
			} catch (ex) {
				reject(new LedServiceException("Failed to post WiFi configuration to the TesLight controller.", ex));
			}
		});
	};

	/**
	 * Query the encoded WiFi configuration from the TesLight controller.
	 * @returns Promise with the base64 encoded data or {@link WiFiServiceException} when the request fails
	 */
	getTeslightWiFiConfiguration = () => {
		return new Promise((resolve, reject) => {
			const url = this.basePath + "config/wifi";

			const options = {
				method: "GET",
				headers: {
					Accept: "octet-stream",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new WiFiServiceException(
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
	 * Post the encoded WiFi configuration to the TesLight controller.
	 * @param {*} wifiConfig WiFi configuration
	 * @returns Promise containing true or {@link WiFiServiceException} when the request fails
	 */
	postTeslightWiFiConfiguration = (wifiConfig) => {
		return new Promise((resolve, reject) => {
			const url = this.basePath + "config/wifi";

			const options = {
				method: "POST",
				headers: {
					"Content-Type": "application/x-www-form-urlencoded",
				},
				body: "data=" + encodeURIComponent(wifiConfig),
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

	/**
	 * Read a {@link string} from the binary config.
	 * @param {object} binConfig buffer storing the binary configuration
	 * @returns next {@link string} from the buffer
	 */
	readString = (binConfig) => {
		const strLen = this.bytesToWord(binConfig.data[0], binConfig.data[1]);

		let string = "";
		for (let i = 0; i < strLen; i++) {
			string += String.fromCharCode(binConfig.data[i + 2]);
		}

		binConfig.data = binConfig.data.slice(strLen + 2);
		return string;
	};

	/**
	 * Read a byte from the binary config.
	 * @param {object} binConfig buffer storing the binary configuration
	 * @returns next byte from the buffer
	 */
	readByte = (binConfig) => {
		const byte = binConfig.data[0];
		binConfig.data = binConfig.data.slice(1);
		return byte;
	};

	/**
	 * Read a boolean from the binary config.
	 * @param {object} binConfig buffer storing the binary configuration
	 * @returns next boolean from the buffer
	 */
	readBoolean = (binConfig) => {
		return this.readByte(binConfig) > 0;
	};

	/**
	 * Read a {@link string} from the binary config.
	 * @param {object} binConfig buffer storing the binary configuration
	 * @param {number} startIndex start index
	 * @param {string} string value to write
	 * @returns new start index
	 */
	writeString = (binConfig, startIndex, string) => {
		const len = this.wordToBytes(string.length);
		binConfig.data[startIndex++] = len.first;
		binConfig.data[startIndex++] = len.second;

		for (let i = 0; i < string.length; i++) {
			binConfig.data[startIndex++] = string.charCodeAt(i);
		}
		return startIndex;
	};

	/**
	 * Read a byte from the binary config.
	 * @param {object} binConfig buffer storing the binary configuration
	 * @param {number} startIndex start index
	 * @param {byte} byte value to write
	 * @returns new start index
	 */
	writeByte = (binConfig, startIndex, byte) => {
		binConfig.data[startIndex++] = byte;
		return startIndex;
	};

	/**
	 * Read a boolean from the binary config.
	 * @param {object} binConfig buffer storing the binary configuration
	 * @param {number} startIndex start index
	 * @param {boolean} bool value to write
	 * @returns new start index
	 */
	writeBoolean = (binConfig, startIndex, bool) => {
		return this.writeByte(binConfig, startIndex, bool === true ? 1 : 0);
	};
}
