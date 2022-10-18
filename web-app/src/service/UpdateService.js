import UpdateServiceException from "../exception/UpdateServiceException";

/**
 * Class contains a service that is used to upload TesLight Update Packages to the TesLight controller.
 */
class UpdateService {
	/**
	 * Create a new instance of the {UpdateService}.
	 * @param {string} url
	 */
	constructor(url) {
		this.url = url;
	}

	/**
	 * Upload a TUP file to the controller.
	 * @param file file from file select dialog
	 */
	uploadTupFile = (file) => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("update");

			const formData = new FormData();
			formData.append("update", file);

			const options = {
				method: "POST",
				headers: {
					Accept: "text/plain",
				},
				body: formData,
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new UpdateServiceException(
							`Failed to upload TesLight Update Package. The status code ${
								response.status
							} implies an error: "${response.text()}"`
						);
					}
					return response.text();
				})
				.then((data) => resolve(parseInt(data)))
				.catch((ex) => reject(ex));
		});
	};
}

export default UpdateService;
