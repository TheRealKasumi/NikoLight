import Exception from "./Exception";

/**
 * Exception thrown by the {UpdateService}.
 */
class UpdateServiceException extends Exception {
	getName = () => {
		return "UpdateServiceException";
	};
}

export default UpdateServiceException;
