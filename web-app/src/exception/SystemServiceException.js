import Exception from "./Exception";

/**
 * Exception thrown by the {SystemSerive}.
 */
class SystemServiceException extends Exception {
	getName = () => {
		return "SystemServiceException";
	};
}

export default SystemServiceException;
