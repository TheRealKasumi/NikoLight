import Exception from "./Exception";

/**
 * Exception thrown when a index is out of bounds.
 */
class OutOfBoundsException extends Exception {
	getName = () => {
		return "OutOfBoundsException";
	};
}

export default OutOfBoundsException;
