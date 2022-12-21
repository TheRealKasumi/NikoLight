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
				systemConfig.setLightSensorThreshold(stream.readByte());
				systemConfig.setLightSensorMinAmbientBrightness(stream.readByte());
				systemConfig.setLightSensorMaxAmbientBrightness(stream.readByte());
				systemConfig.setLightSensorMinLedBrightness(stream.readByte());
				systemConfig.setLightSensorMaxLedBrightness(stream.readByte());
				systemConfig.setLightSensorDuration(stream.readByte());
				systemConfig.setSystemPowerLimit(stream.readByte());
				systemConfig.setRegulatorHighTemperature(stream.readByte());
				systemConfig.setRegulatorCutoffTemperature(stream.readByte());
				systemConfig.setFanMinPwmValue(stream.readByte());
				systemConfig.setFanMaxPwmValue(stream.readByte());
				systemConfig.setFanMinTemperature(stream.readByte());
				systemConfig.setFanMaxTemperature(stream.readByte());
				systemConfig.hasChanged(true);
				resolve(systemConfig);
			} catch (ex) {
				reject(new SystemServiceException("Failed to read binary data from the stream."));
			}
		});
	};

	/**
	 * Send the system configuration to the TesLight controller.
	 * @param {SystemConfiguration} systemConfig system configuration to send to the TesLight controller
	 */
	postSystemConfiguration = (systemConfig) => {
		return new Promise(async (resolve, reject) => {
			const stream = new BinaryStream(15);

			try {
				stream.writeByte(systemConfig.getLogLevel());
				stream.writeByte(systemConfig.getLightSensorMode());
				stream.writeByte(systemConfig.getLightSensorThreshold());
				stream.writeByte(systemConfig.getLightSensorMinAmbientBrightness());
				stream.writeByte(systemConfig.getLightSensorMaxAmbientBrightness());
				stream.writeByte(systemConfig.getLightSensorMinLedBrightness());
				stream.writeByte(systemConfig.getLightSensorMaxLedBrightness());
				stream.writeByte(systemConfig.getLightSensorDuration());
				stream.writeByte(systemConfig.getSystemPowerLimit());
				stream.writeByte(systemConfig.getRegulatorHighTemperature());
				stream.writeByte(systemConfig.getRegulatorCutoffTemperature());
				stream.writeByte(systemConfig.getFanMinPwmValue());
				stream.writeByte(systemConfig.getFanMaxPwmValue());
				stream.writeByte(systemConfig.getFanMinTemperature());
				stream.writeByte(systemConfig.getFanMaxTemperature());
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
}

export default SystemService;
