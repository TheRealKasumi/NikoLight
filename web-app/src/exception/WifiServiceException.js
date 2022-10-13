import Exception from "./Exception";

/**
 * Exception thrown by the {WifiService}.
 */
class WifiServiceException extends Exception {
	getName = () => {
		return "WifiServiceException";
	};
}

export default WifiServiceException;
