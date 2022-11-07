/**
 * Class for holding the system configuration of the TesLight controller in the frontend.
 */
class SystemConfiguration {
	/**
	 * Create a new instance of {SystemConfiguration} and initialize it with defaults.
	 */
	constructor() {
		this.changed = false;
		this.logLevel = 1;
		this.lightSensorMode = 1;
		this.lightSensorThreshold = 30;
		this.lightSensorMinValue = 30;
		this.lightSensorMaxValue = 2048;
		this.systemPowerLimit = 10;
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
	 * Get the log level.
	 * @returns log level
	 */
	getLogLevel = () => {
		return this.logLevel;
	};

	/**
	 * Get the light sensor mode.
	 * @returns light sensor mode
	 */
	getLightSensorMode = () => {
		return this.lightSensorMode;
	};

	/**
	 * Get the light sensor threshold.
	 * @returns light sensor threshold
	 */
	getLightSensorThreshold = () => {
		return this.lightSensorThreshold;
	};

	/**
	 * Get the light sensor min value.
	 * @returns light sensor min value
	 */
	getLightSensorMinValue = () => {
		return this.lightSensorMinValue;
	};

	/**
	 * Get the light sensor max value.
	 * @returns light sensor max value
	 */
	getLightSensorMaxValue = () => {
		return this.lightSensorMaxValue;
	};

	/**
	 * Get the system power limit in W.
	 * @returns system power limit
	 */
	getSystemPowerLimit = () => {
		return this.systemPowerLimit;
	};

	/**
	 * Set the log level.
	 * @param {number} logLevel log level
	 * @returns true when valid, false when invalid
	 */
	setLogLevel = (logLevel) => {
		if (typeof logLevel === "number" && logLevel >= 0 && logLevel < 4) {
			this.logLevel = logLevel;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the light sensor mode.
	 * @param {number} lightSensorMode light sensor mode
	 * @returns true when valid, false when invalid
	 */
	setLightSensorMode = (lightSensorMode) => {
		if (typeof lightSensorMode === "number" && lightSensorMode >= 0 && lightSensorMode < 4) {
			this.lightSensorMode = lightSensorMode;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the light sensor threshold.
	 * @param {number} lightSensorThreshold light sensor threshold
	 * @returns true when valid, false when invalid
	 */
	setLightSensorThreshold = (lightSensorThreshold) => {
		if (typeof lightSensorThreshold === "number" && lightSensorThreshold >= 0 && lightSensorThreshold < 4096) {
			this.lightSensorThreshold = lightSensorThreshold;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the light sensor minimum value
	 * @param {number} lightSensorMinValue ligght sensor minimum value
	 * @returns true when valid, false when invalid
	 */
	setLightSensorMinValue = (lightSensorMinValue) => {
		if (
			typeof lightSensorMinValue === "number" &&
			lightSensorMinValue >= 0 &&
			lightSensorMinValue < 4096 &&
			lightSensorMinValue < this.lightSensorMaxValue
		) {
			this.lightSensorMinValue = lightSensorMinValue;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the light sensor maximum value
	 * @param {number} lightSensorMaxValue light sensor maximum value
	 * @returns true when valid, false when invalid
	 */
	setLightSensorMaxValue = (lightSensorMaxValue) => {
		if (
			typeof lightSensorMaxValue === "number" &&
			lightSensorMaxValue >= 0 &&
			lightSensorMaxValue < 4096 &&
			lightSensorMaxValue > this.lightSensorMinValue
		) {
			this.lightSensorMaxValue = lightSensorMaxValue;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the system power limit in W.
	 * @param systemPowerLimit system power limit in W
	 * @returns true when valid, false when invalid
	 */
	setSystemPowerLimit = (systemPowerLimit) => {
		if (typeof systemPowerLimit === "number" && systemPowerLimit >= 0 && systemPowerLimit < 256) {
			this.systemPowerLimit = systemPowerLimit;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Copy the values from a {SystemConfiguration}.
	 */
	copyFrom = (systemConfiguration) => {
		this.changed = systemConfiguration.changed;
		this.logLevel = systemConfiguration.logLevel;
		this.lightSensorMode = systemConfiguration.lightSensorMode;
		this.lightSensorThreshold = systemConfiguration.lightSensorThreshold;
		this.lightSensorMinValue = systemConfiguration.lightSensorMinValue;
		this.lightSensorMaxValue = systemConfiguration.lightSensorMaxValue;
		this.systemPowerLimit = systemConfiguration.systemPowerLimit;
	};

	/**
	 * Create a clone of the {SystemConfiguration}.
	 * @returns clone
	 */
	clone = () => {
		const clone = new SystemConfiguration();
		clone.changed = this.changed;
		clone.logLevel = this.logLevel;
		clone.lightSensorMode = this.lightSensorMode;
		clone.lightSensorThreshold = this.lightSensorThreshold;
		clone.lightSensorMinValue = this.lightSensorMinValue;
		clone.lightSensorMaxValue = this.lightSensorMaxValue;
		clone.systemPowerLimit = this.systemPowerLimit;
		return clone;
	};
}

export default SystemConfiguration;
