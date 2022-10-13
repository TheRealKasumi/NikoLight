/**
 * Class for holding the WiFi configuration of the TesLight controller in the frontend.
 */
class WifiConfiguration {
	/**
	 * Create a new instance of {WifiConfiguration} and initialize it with defaults.
	 */
	constructor() {
		this.changed = false;
		this.accessPointSsid = "TesLight";
		this.accessPointPassword = "TesLightPW";
		this.accessPointChannel = 1;
		this.accessPointHidden = false;
		this.accessPointMaxConnections = 1;
		this.wifiSsid = "";
		this.wifiPassword = "";
	}

	/**
	 * Return if the configuration has changed since last checked.
	 * @param {boolean} reset reset the has changed state
	 * @returns true when changed, false otherwise
	 */
	hasChanged = (reset) => {
		const changed = this.changed;
		if (this.changed === true && reset === true) {
			this.changed = false;
		}
		return changed;
	};

	/**
	 * Get the access point SSID.
	 * @returns SSID of the access point
	 */
	getAccessPointSsid = () => {
		return this.accessPointSsid;
	};

	/**
	 * Get the access point password.
	 * @returns password of the access point
	 */
	getAccessPointPassword = () => {
		return this.accessPointPassword;
	};

	/**
	 * Get the channel of the access point.
	 * @returns access point channel
	 */
	getAccessPointChannel = () => {
		return this.accessPointChannel;
	};

	/**
	 * Check if the access point ssid is hidden or visiable
	 * @returns hidden or visible
	 */
	getAccessPointHidden = () => {
		return this.accessPointHidden;
	};

	/**
	 * Get the maximum number of connections to the access point.
	 * @returns maximum number of connection to the access point
	 */
	getAccessPointMaxConnections = () => {
		return this.accessPointMaxConnections;
	};

	/**
	 * Get the ssid of the WiFi network.
	 * @returns ssid of the WiFi network
	 */
	getWifiSsid = () => {
		return this.wifiSsid;
	};

	/**
	 * Get the password of the WiFi network.
	 * @returns password of the WiFi network
	 */
	getWifiPassword = () => {
		return this.wifiPassword;
	};

	/**
	 * Set the access point SSID.
	 * @param {string} ssid SSID of the access point
	 * @returns true when valid, false when invalid
	 */
	setAccessPointSsid = (ssid) => {
		if (typeof ssid === "string" && this.isValidSsid(ssid) === true) {
			this.accessPointSsid = ssid;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the access point password.
	 * @param {string} password password of the access point
	 * @returns true when valid, false when invalid
	 */
	setAccessPointPassword = (password) => {
		if (typeof password === "string" && this.isValidPassword(password) === true) {
			this.accessPointPassword = password;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the access point channel.
	 * @param {number} channel channel of the access point
	 * @returns true when valid, false when invalid
	 */
	setAccessPointChannel = (channel) => {
		if (typeof channel === "number" && channel > 0 && channel < 13) {
			this.accessPointChannel = channel;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set if the SSID of the access point is hidden.
	 * @param {boolean} hidden set if the access point SSID is hidden
	 * @returns true when valid, false when invalid
	 */
	setAccessPointHidden = (hidden) => {
		if (typeof hidden === "boolean") {
			this.accessPointHidden = hidden;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the maximum number of connections to the access point.
	 * @param {number} maxConnections maximum number of connections
	 * @returns true when valid, false when invalid
	 */
	setAccessPointMaxConnections = (maxConnections) => {
		if (typeof maxConnections === "number" && maxConnections > 0 && maxConnections < 5) {
			this.accessPointMaxConnections = maxConnections;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the SSID of the router to connect to.
	 * @param {string} ssid SSID of the router
	 * @returns true when valid, false when invalid
	 */
	setWifiSsid = (ssid) => {
		if (typeof ssid === "string" && this.isValidSsid(ssid) === true) {
			this.wifiSsid = ssid;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the password of the router to connect to.
	 * @param {string} password of the router
	 * @returns true when valid, false when invalid
	 */
	setWifiPassword = (password) => {
		if (typeof password === "string" && this.isValidPassword(password) === true) {
			this.wifiPassword = password;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Validate if the ssid if valid
	 * @param {string} ssid
	 */
	isValidSsid = (ssid) => {
		if (ssid.length < 2 || ssid.length > 32) {
			return false;
		}

		const validChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_ ";
		for (let i = 0; i < ssid.length; i++) {
			if (validChars.indexOf(ssid[i]) === -1) {
				return false;
			}
		}
		return true;
	};

	/**
	 * Validate if the password if valid
	 * @param {string} password
	 */
	isValidPassword = (password) => {
		if (password.length < 8 || password.length > 63) {
			return false;
		}

		const validChars = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+-_!"§$%&/()=?*# ';
		for (let i = 0; i < password.length; i++) {
			if (validChars.indexOf(password[i]) === -1) {
				return false;
			}
		}
		return true;
	};

	/**
	 * Copy the values from a {WiFiConfiguration}.
	 */
	copyFrom = (wifiConfiguration) => {
		this.changed = wifiConfiguration.changed;
		this.accessPointSsid = wifiConfiguration.accessPointSsid;
		this.accessPointPassword = wifiConfiguration.accessPointPassword;
		this.accessPointChannel = wifiConfiguration.accessPointChannel;
		this.accessPointHidden = wifiConfiguration.accessPointHidden;
		this.accessPointMaxConnections = wifiConfiguration.accessPointMaxConnections;
		this.wifiSsid = wifiConfiguration.wifiSsid;
		this.wifiPassword = wifiConfiguration.wifiSsid;
	};

	/**
	 * Create a clone of the {WifiConfiguration}.
	 * @returns clone
	 */
	clone = () => {
		const clone = new WifiConfiguration();
		clone.changed = this.changed;
		clone.accessPointSsid = this.accessPointSsid;
		clone.accessPointPassword = this.accessPointPassword;
		clone.accessPointChannel = this.accessPointChannel;
		clone.accessPointHidden = this.accessPointHidden;
		clone.accessPointMaxConnections = this.accessPointMaxConnections;
		clone.wifiSsid = this.wifiSsid;
		clone.wifiPassword = this.wifiSsid;
		return clone;
	};
}

export default WifiConfiguration;
