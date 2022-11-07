import FseqException from "../exception/FseqException";

/**
 * Class contains a service to manage the fseq files on the TesLight controller.
 */
class FseqService {
	/**
	 * Create a new instance of the {FseqService}.
	 * @param {string} url
	 */
	constructor(url) {
		this.url = url;
	}

	/**
	 * Get a list of currently stored fseq files from the controller.
	 */
	getFileList = () => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("fseq");

			const options = {
				method: "GET",
				headers: {
					Accept: "text/plain",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new FseqException(
							`Failed to get fseq file list. The status code ${
								response.status
							} implies an error: "${response.text()}"`
						);
					}

					return response.text();
				})
				.then((data) => {
					const fileList = [];
					const lines = data.split("\n");
					for (let i = 0; i < lines.length; i++) {
						if (lines[i] !== "") {
							const parts = lines[i].split(";");
							if (parts.length !== 3) {
								throw new FseqException("Failed to parse fseq file list.");
							}

							fileList.push({
								fileName: parts[0],
								fileSize: parts[1],
								fileId: parts[2],
							});
						}
					}
					resolve(fileList);
				})
				.catch((ex) => reject(ex));
		});
	};

	/**
	 * Upload a new fseq file to the controller.
	 * @param file file from file select dialog
	 * @param {string} fileName name of the file on the controller
	 */
	postFseqFile = (file, fileName) => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("fseq?fileName=").concat(fileName);

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
						throw new FseqException(
							`Failed to upload fseq file. The status code ${
								response.status
							} implies an error: "${response.text()}"`
						);
					}

					return response.text();
				})
				.then((data) => resolve(data))
				.catch((ex) => reject(ex));
		});
	};

	/**
	 * Delete a fseq file by it's name.
	 * @param {string} fileName name of the file on the controller
	 */
	deleteFseqFile = (fileName) => {
		return new Promise((resolve, reject) => {
			const url = this.url.concat("fseq?fileName=").concat(fileName);

			const options = {
				method: "DELETE",
				headers: {
					Accept: "text/plain",
				},
			};

			fetch(url, options)
				.then((response) => {
					if (response.status !== 200) {
						throw new FseqException(
							`Failed to delete fseq file. The status code ${
								response.status
							} implies an error: "${response.text()}"`
						);
					}

					return response.text();
				})
				.then((data) => resolve(data))
				.catch((ex) => reject(ex));
		});
	};
}

export default FseqService;
