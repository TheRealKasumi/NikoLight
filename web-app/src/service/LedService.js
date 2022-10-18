import LedConfiguration from "../model/LedConfiguration";
import LedServiceException from "../exception/LedServiceException";
import BinaryStream from "../util/BinaryStream";

/**
 * Class contains a service that is used to query the LED endpoint of the
 * TesLight controller and send changes in the configuration.
 */
class LedService {
	/**
	 * Create a new instance of the {LedService}.
	 * @param {string} url
	 */
	constructor(url) {
		this.url = url;
	}

	/**
	 * Get the decoded LED configuration from the TesLight controller.
	 */
	getLedConfiguration = () => {
		return new Promise(async (resolve, reject) => {
			let encodedData;
			try {
				encodedData = await this.getEncodedLedConfiguration();
			} catch (ex) {
				reject(new LedServiceException("Failed to query LED configuration from the TesLight controller.", ex));
				return;
			}

			const stream = new BinaryStream();
			stream.loadFromBase64(encodedData);

			try {
				const zones = [];
				for (let i = 0; i < 6; i++) {
					const zone = new LedConfiguration();
					zone.setLedPin(stream.readByte());
					zone.setLedCount(stream.readWord());
					zone.setType(stream.readByte());
					zone.setSpeed(stream.readByte());
					zone.setOffset(stream.readWord());
					zone.setBrightness(stream.readByte());
					zone.setReverse(stream.readByte() >= 1);
					zone.setFadeSpeed(stream.readByte());
					const customFields = zone.getCustomFields();
					for (let j = 0; j < 10; j++) {
						customFields[j] = stream.readByte();
					}
					zone.setCustomFields(customFields);
					zone.hasChanged(true);
					zones.push(zone);
				}
				resolve(zones);
			} catch (ex) {
				reject(new LedServiceException("Failed to read binary data from the stream."));
			}
		});
	};

	/**
	 * Send the LED configuration to the TesLight controller.
	 * @param {Array} ledConfiguration array of {LedConfiguration} to send to the TesLight controller
	 */
	postLedConfiguration = (ledConfiguration) => {
		return new Promise(async (resolve, reject) => {
			const stream = new BinaryStream(120);

			try {
				for (let i = 0; i < 6; i++) {
					const zone = ledConfiguration[i];

					stream.writeByte(zone.getLedPin());
					stream.writeWord(zone.getLedCount());
					stream.writeByte(zone.getType());
					stream.writeByte(zone.getSpeed());
					stream.writeWord(zone.getOffset());
					stream.writeByte(zone.getBrightness());
					stream.writeByte(zone.getReverse());
					stream.writeByte(zone.getFadeSpeed());

					const customFields = zone.getCustomFields();
					for (let j = 0; j < 10; j++) {
						stream.writeByte(customFields[j]);
					}
				}
			} catch (ex) {
				reject(new LedServiceException("Failed to write binary data to the stream."));
				return;
			}

			try {
				const result = await this.postEncodedLedConfiguration(stream.saveToBase64());
				resolve(result);
			} catch (ex) {
				reject(new LedServiceException("Failed to send LED configuration to the TesLight controller.", ex));
			}
		});
	};

	/**
	 * Query the encoded LED configuration from the TesLight controller.
	 */
	getEncodedLedConfiguration = () => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("config/led");

			const options = {
				method: "GET",
				headers: {
					Accept: "application/octet-stream",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new LedServiceException(
							`The status code ${response.status} implies an error: "${response.text()}"`
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
	 * @param {string} ledConfiguration base64 encoded {LedConfiguration}
	 */
	postEncodedLedConfiguration = (ledConfiguration) => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("config/led");

			const options = {
				method: "POST",
				headers: {
					"Content-Type": "application/x-www-form-urlencoded",
				},
				body: "data=" + encodeURIComponent(ledConfiguration),
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new LedServiceException(
							`The status code ${response.status} implies an error: "${response.text()}"`
						);
					}

					return response.text();
				})
				.then((data) => resolve(data))
				.catch((ex) => reject(ex));
		});
	};
}

export default LedService;
