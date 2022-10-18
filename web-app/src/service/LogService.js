import LogServiceException from "../exception/LogServiceException";

/**
 * Class contains a service that is used to query the log endpoint of the TesLight controller.
 */
class LogService {
	/**
	 * Create a new instance of the {LogService}.
	 * @param {string} url
	 */
	constructor(url) {
		this.url = url;
	}

	/**
	 * Get the current size of the log on the controller in bytes.
	 */
	getLogSize = () => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("log/size");

			const options = {
				method: "GET",
				headers: {
					Accept: "text/plain",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new LogServiceException(
							`Failed to get log size. The status code ${response.status} implies an error: "${response.text()}"`
						);
					}

					return response.text();
				})
				.then((data) => resolve(parseInt(data)))
				.catch((ex) => reject(ex));
		});
	};

	/**
	 * Get the log from the TesLight controller.
	 * @param {*} startByte index of the start byte
	 * @param {*} byteCount number of bytes to read
	 */
	getLog = (startByte, byteCount) => {
		return new Promise((resolve, reject) => {
			const url = `${this.url}log?start=${startByte}&count=${byteCount}`;

			const options = {
				method: "GET",
				headers: {
					Accept: "text/plain",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new LogServiceException(
							`Failed to get log. The status code ${response.status} implies an error: "${response.text()}"`
						);
					}

					return response.text();
				})
				.then((data) => resolve(data))
				.catch((ex) => reject(ex));
		});
	};

	/**
	 * Clear the log on the TesLight controller.
	 */
	clearLog = () => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("log");

			const options = {
				method: "DELETE",
				headers: {
					Accept: "text/plain",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new LogServiceException(
							`Failed to clear log. The status code ${response.status} implies an error: "${response.text()}"`
						);
					}

					return response.text();
				})
				.then((data) => resolve(data))
				.catch((ex) => reject(ex));
		});
	};
}

export default LogService;
