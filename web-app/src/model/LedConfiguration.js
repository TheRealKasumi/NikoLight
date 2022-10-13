/**
 * Class for holding the led configuration of a single channel in the frontend.
 */
class LedConfiguration {
	/**
	 * Create a new instance of {LedConfiguration} and initialize it with defaults.
	 */
	constructor() {
		this.changed = false;
		this.ledPin = 13;
		this.ledCount = 1;
		this.type = 0;
		this.speed = 50;
		this.offset = 10;
		this.brightness = 50;
		this.reverse = false;
		this.fadeSpeed = 5;
		this.customFields = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
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
	 * Get the pin number of the LED port.
	 * @returns pin number of the LED port
	 */
	getLedPin = () => {
		return this.ledPin;
	};

	/**
	 * Get the number of LEDs.
	 * @returns number of LEDs
	 */
	getLedCount = () => {
		return this.ledCount;
	};

	/**
	 * Get the animation type used for the LEDs.
	 * @returns animation type for the LEDs
	 */
	getType = () => {
		return this.type;
	};

	/**
	 * Get the speed of the LED animation.
	 * @returns speed of the LED animation
	 */
	getSpeed = () => {
		return this.speed;
	};

	/**
	 * Get the offset of the LED animaiton.
	 * @returns offset of the LED animaiton
	 */
	getOffset = () => {
		return this.offset;
	};

	/**
	 * Get the brightness of the LED animation.
	 * @returns brightness of the LED animation
	 */
	getBrightness = () => {
		return this.brightness;
	};

	/**
	 * Get the reversion of the LED animation.
	 * @returns reversion of the LED animation
	 */
	getReverse = () => {
		return this.reverse;
	};

	/**
	 * Get the fading speed for the LEDs.
	 * @returns fading speed for the LEDs.
	 */
	getFadeSpeed = () => {
		return this.fadeSpeed;
	};

	/**
	 * Get the custom fields of the LEDs.
	 * @returns custom fields of the LEDs
	 */
	getCustomFields = () => {
		return this.customFields;
	};

	/**
	 * Set the output pin for the LEDs.
	 * @param {number} ledPin
	 * @returns true when valid, false when invalid
	 */
	setLedPin = (ledPin) => {
		if (typeof ledPin === "number" && ((ledPin >= 13 && ledPin <= 17) || ledPin === 21)) {
			this.ledPin = ledPin;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the number of LEDs.
	 * @param {number} ledCount number of LEDs
	 * @returns true when valid, false when invalid
	 */
	setLedCount = (ledCount) => {
		if (typeof ledCount === "number" && ledCount > 0 && ledCount <= 255) {
			this.ledCount = ledCount;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the type of the animation.
	 * @param {number} type animation type
	 * @returns true when valid, false when invalid
	 */
	setType = (type) => {
		if (typeof type === "number" && ((type >= 0 && type <= 4) || type === 255)) {
			this.type = type;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the speed of the animation.
	 * @param {*} speed animation speed
	 * @returns true when valid, false when invalid
	 */
	setSpeed = (speed) => {
		if (typeof speed === "number" && speed >= 0 && speed <= 255) {
			this.speed = speed;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Get the offset of the animation
	 * @param {number} offset animation offset
	 * @returns true when valid, false when invalid
	 */
	setOffset = (offset) => {
		if (typeof offset === "number" && offset >= 0 && offset <= 255) {
			this.offset = offset;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the brightness of the animation.
	 * @param {number} brightness animation brightness
	 * @returns true when valid, false when invalid
	 */
	setBrightness = (brightness) => {
		if (typeof brightness === "number" && brightness >= 0 && brightness <= 255) {
			this.brightness = brightness;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the reversion of the animation.
	 * @param {boolean} reverse yes/no
	 * @returns true when valid, false when invalid
	 */
	setReverse = (reverse) => {
		if (typeof reverse === "boolean") {
			this.reverse = reverse;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the fading speed of the animation.
	 * Technically 0 is possible but makes no sense.
	 * @param {number} fadeSpeed fading speed of the animation
	 * @returns true when valid, false when invalid
	 */
	setFadeSpeed = (fadeSpeed) => {
		if (typeof fadeSpeed === "number" && fadeSpeed >= 0 && fadeSpeed <= 255) {
			this.fadeSpeed = fadeSpeed;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Set the custom fields of the animaiton.
	 * The meaning depends on the used animation type.
	 * @param {object} customFields custom fields
	 * @returns true when valid, false when invalid
	 */
	setCustomFields = (customFields) => {
		if (typeof customFields === "object" && Array.isArray(customFields) && customFields.length === 10) {
			this.customFields = customFields;
			this.changed = true;
			return true;
		}
		return false;
	};

	/**
	 * Copy the values from a {LedConfiguration}.
	 */
	copyFrom = (ledConfiguration) => {
		this.changed = ledConfiguration.changed;
		this.ledPin = ledConfiguration.ledPin;
		this.ledCount = ledConfiguration.ledCount;
		this.type = ledConfiguration.type;
		this.speed = ledConfiguration.speed;
		this.offset = ledConfiguration.offset;
		this.brightness = ledConfiguration.brightness;
		this.reverse = ledConfiguration.reverse;
		this.fadeSpeed = ledConfiguration.fadeSpeed;
		for (let i = 0; i < ledConfiguration.customFields.length; i++) {
			this.customFields[i] = ledConfiguration.customFields[i];
		}
	};

	/**
	 * Create a clone of the {LedConfiguration}.
	 * @returns clone
	 */
	clone = () => {
		const clone = new LedConfiguration();
		clone.changed = this.changed;
		clone.ledPin = this.ledPin;
		clone.ledCount = this.ledCount;
		clone.type = this.type;
		clone.speed = this.speed;
		clone.offset = this.offset;
		clone.brightness = this.brightness;
		clone.reverse = this.reverse;
		clone.fadeSpeed = this.fadeSpeed;
		for (let i = 0; i < this.customFields.length; i++) {
			clone.customFields[i] = this.customFields[i];
		}
		return clone;
	};
}

export default LedConfiguration;
