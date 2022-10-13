import Exception from "./Exception";

/**
 * Exception thrown by the {LedService}.
 */
class LedServiceException extends Exception {
	getName = () => {
		return "LedServiceException";
	};
}

export default LedServiceException;
