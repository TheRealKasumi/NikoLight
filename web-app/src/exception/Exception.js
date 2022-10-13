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

export default Exception;
