/**
 * Class contains static functions to convert base64 encoded data
 * into a {Uint8Array} and the other way around.
 */
class Base64Transcoder {
	/**
	 * Decode a base64 string and return it's binary data as {Uint8Array}.
	 * @param {string} base64 base64 encoded data
	 * @returns {Uint8Array} holding the decoded binary data
	 */
	static decode = (base64) => {
		return Uint8Array.from(atob(base64), (c) => c.charCodeAt(0));
	};

	/**
	 * Enocde an {Uint8Array} with binary data into a base64 encoded string.
	 * @param {Uint8Array} binary array containing the binary data
	 * @returns base64 encoded string
	 */
	static encode = (binary) => {
		return btoa(String.fromCharCode.apply(null, binary));
	};
}

export default Base64Transcoder;
