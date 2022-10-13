import WifiConfiguration from "../model/WifiConfiguration";
import WifiServiceException from "../exception/WifiServiceException";
import BinaryStream from "../util/BinaryStream";

/**
 * Class contains a service that is used to query the WiFi configuration endpoint of the
 * TesLight controller and send changes in the configuration.
 */
class WifiService {
	/**
	 * Create a new instance of the {WifiService}.
	 * @param {string} url
	 */
	constructor(url) {
		this.url = url;
	}

	/**
	 * Get the decoded {WifiConfiguration} from the TesLight controller.
	 */
	getWifiConfiguration = () => {
		return new Promise(async (resolve, reject) => {
			let encodedData;
			try {
				encodedData = await this.getEncodedWifiConfiguration();
			} catch (ex) {
				reject(new WifiServiceException("Failed to query WiFi configuration from the TesLight controller.", ex));
				return;
			}

			const stream = new BinaryStream();
			stream.loadFromBase64(encodedData);

			try {
				const wifiConfig = new WifiConfiguration();
				wifiConfig.setAccessPointSsid(stream.readString());
				wifiConfig.setAccessPointPassword(stream.readString());
				wifiConfig.setAccessPointHidden(stream.readByte());
				wifiConfig.setAccessPointMaxConnections(stream.readByte());
				wifiConfig.setWifiSsid(stream.readString());
				wifiConfig.setWifiPassword(stream.readString());
				wifiConfig.hasChanged(true);
				resolve(wifiConfig);
			} catch (ex) {
				reject(new WifiServiceException("Failed to read binary data from the stream."));
			}
		});
	};

	/**
	 * Send the WiFi configuration to the TesLight controller.
	 * @param {WifiConfiguration} wifiConfiguration wifi configuration to send to the TesLight controller
	 */
	postWifiConfiguration = (wifiConfiguration) => {
		return new Promise(async (resolve, reject) => {
			let length = 3;
			length += wifiConfiguration.getAccessPointSsid().length + 2;
			length += wifiConfiguration.getAccessPointPassword().length + 2;
			length += wifiConfiguration.getWifiSsid().length + 2;
			length += wifiConfiguration.getWifiPassword().length + 2;
			const stream = new BinaryStream(length);

			try {
				stream.writeString(wifiConfiguration.getAccessPointSsid());
				stream.writeString(wifiConfiguration.getAccessPointPassword());
				stream.writeByte(wifiConfiguration.getAccessPointChannel());
				stream.writeByte(wifiConfiguration.getAccessPointHidden());
				stream.writeByte(wifiConfiguration.getAccessPointMaxConnections());
				stream.writeString(wifiConfiguration.getWifiSsid());
				stream.writeString(wifiConfiguration.getWifiPassword());
			} catch (ex) {
				reject(new WifiServiceException("Failed to write binary data to the stream."));
				return;
			}

			try {
				const result = await this.postEncodedWifiConfiguration(stream.saveToBase64());
				console.log(result);
				resolve(result);
			} catch (ex) {
				reject(new WifiServiceException("Failed to send WiFi configuration to the TesLight controller.", ex));
			}
		});
	};

	/**
	 * Query the encoded WiFi configuration from the TesLight controller.
	 */
	getEncodedWifiConfiguration = () => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("config/wifi");

			const options = {
				method: "GET",
				headers: {
					Accept: "application/octet-stream",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new WifiServiceException(
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
	 * Post the encoded WiFi configuration to the TesLight controller.
	 * @param {string} wifiConfiguration base64 encoded {WifiConfiguration}
	 */
	postEncodedWifiConfiguration = (wifiConfiguration) => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("config/wifi");

			const options = {
				method: "POST",
				headers: {
					"Content-Type": "application/x-www-form-urlencoded",
				},
				body: "data=" + encodeURIComponent(wifiConfiguration),
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 202) {
						throw new WifiServiceException(
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

export default WifiService;
