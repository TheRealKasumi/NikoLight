import Exception from "./Exception";

/**
 * Exception thrown by the {FseqService}.
 */
class FseqException extends Exception {
	getName = () => {
		return "FseqException";
	};
}

export default FseqException;
