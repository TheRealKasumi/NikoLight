import SystemConfiguration from "../model/SystemConfiguration";
import SystemServiceException from "../exception/SystemServiceException";
import BinaryStream from "../util/BinaryStream";

/**
 * Class contains a service that is used to query the system configuration endpoint of the
 * TesLight controller and send changes in the configuration.
 */
class SystemService {
	/**
	 * Create a new instance of the {SystemService}.
	 * @param {string} url
	 */
	constructor(url) {
		this.url = url;
	}

	/**
	 * Get the decoded {SystemConfiguration} from the TesLight controller.
	 */
	getSystemConfiguration = () => {
		return new Promise(async (resolve, reject) => {
			let encodedData;
			try {
				encodedData = await this.getEncodedSystemConfiguration();
			} catch (ex) {
				reject(
					new SystemServiceException("Failed to query system configuration from the TesLight controller.", ex)
				);
				return;
			}

			const stream = new BinaryStream();
			stream.loadFromBase64(encodedData);

			try {
				const systemConfig = new SystemConfiguration();
				systemConfig.setLogLevel(stream.readByte());
				systemConfig.setLightSensorMode(stream.readByte());
				systemConfig.setLightSensorThreshold(stream.readWord());
				systemConfig.setLightSensorMinValue(stream.readWord());
				systemConfig.setLightSensorMaxValue(stream.readWord());
				systemConfig.setSystemPowerLimit(stream.readByte());
				systemConfig.setLedVoltage(stream.readByte());
				systemConfig.setLedChannelCurrent([stream.readByte(), stream.readByte(), stream.readByte()]);
				systemConfig.hasChanged(true);
				resolve(systemConfig);
			} catch (ex) {
				reject(new SystemServiceException("Failed to read binary data from the stream."));
			}
		});
	};

	/**
	 * Send the system configuration to the TesLight controller.
	 * @param {SystemConfiguration} systemConfiguration system configuration to send to the TesLight controller
	 */
	postSystemConfiguration = (systemConfiguration) => {
		return new Promise(async (resolve, reject) => {
			const stream = new BinaryStream(13);

			try {
				stream.writeByte(systemConfiguration.getLogLevel());
				stream.writeByte(systemConfiguration.getLightSensorMode());
				stream.writeWord(systemConfiguration.getLightSensorThreshold());
				stream.writeWord(systemConfiguration.getLightSensorMinValue());
				stream.writeWord(systemConfiguration.getLightSensorMaxValue());
				stream.writeByte(systemConfiguration.getSystemPowerLimit());
				stream.writeByte(systemConfiguration.getLedVoltage());
				stream.writeByte(systemConfiguration.getLedChannelCurrent()[0]);
				stream.writeByte(systemConfiguration.getLedChannelCurrent()[1]);
				stream.writeByte(systemConfiguration.getLedChannelCurrent()[2]);
			} catch (ex) {
				reject(new SystemServiceException("Failed to write binary data to the stream."));
				return;
			}

			try {
				const result = await this.postEncodedSystemConfiguration(stream.saveToBase64());
				resolve(result);
			} catch (ex) {
				reject(new SystemServiceException("Failed to send system configuration to the TesLight controller.", ex));
			}
		});
	};

	/**
	 * Query the encoded system configuration from the TesLight controller.
	 */
	getEncodedSystemConfiguration = () => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("config/system");

			const options = {
				method: "GET",
				headers: {
					Accept: "application/octet-stream",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new SystemServiceException(
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
	 * Post the encoded system configuration to the TesLight controller.
	 * @param {string} systemConfiguration base64 encoded {SystemConfiguration}
	 */
	postEncodedSystemConfiguration = (systemConfiguration) => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("config/system");

			const options = {
				method: "POST",
				headers: {
					"Content-Type": "application/x-www-form-urlencoded",
				},
				body: "data=" + encodeURIComponent(systemConfiguration),
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 202) {
						throw new SystemServiceException(
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

export default SystemService;
