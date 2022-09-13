/**
 * @file Exception.js
 * @author TheRealKasumi
 * @brief Contains exceptions.
 * @version 0.0.1
 * @date 2022-07-22
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * Base class for all exceptions.
 */
class Exception {
	constructor(message, cause) {
		this.message = message;
		this.cause = cause;
	}

	toString = (depth) => {
		if (typeof depth !== "number" || depth < 0) {
			depth = 0;
		}

		let message = depth === 0 ? "The following error occurred:\n" : "";
		message += "[" + this.getName() + "]" + ': "' + this.message + '"';

		if (this.cause) {
			message += "\n";
			for (let i = 0; i < depth + 1; i++) {
				message += "   ";
			}
			message += "caused by: " + this.cause.toString(depth + 1);
		}

		return message;
	};

	getName = () => {
		return "Exception";
	};
}

/**
 * Exception thrown by the {@link SystemSerive}.
 */
 class SystemServiceException extends Exception {
	getName = () => {
		return "SystemServiceException";
	};
}

/**
 * Exception thrown by the {@link LedService}.
 */
class LedServiceException extends Exception {
	getName = () => {
		return "LedServiceException";
	};
}

/**
 * Exception thrown by the {@link WiFiService}.
 */
class WiFiServiceException extends Exception {
	getName = () => {
		return "WiFiServiceException";
	};
}
