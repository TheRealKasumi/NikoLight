import Exception from "./Exception";

/**
 * Exception thrown by the {LogService}.
 */
class LogServiceException extends Exception {
	getName = () => {
		return "LogServiceException";
	};
}

export default LogServiceException;
