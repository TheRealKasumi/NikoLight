import Base64Transcoder from "./Base64Transcoder";
import OutOfBoundsException from "../exception/OutOfBoundsException";

/**
 * Class for handling binary data in form of a stream.
 */
class BinaryStream {
	/**
	 * Create a new instance of {BinaryStream}.
	 * @param {number} size size of the stream in bytes
	 */
	constructor(size) {
		if (typeof size === "number" && size > 0) {
			this.binaryData = new Uint8Array(size);
		} else {
			this.binaryData = null;
		}
		this.position = 0;
	}

	/**
	 * Get the size of the stream in bytes.
	 * @returns size of the stream in bytes
	 */
	getSize = () => {
		if (this.binaryData !== null) {
			return this.binaryData.length;
		}
		return 0;
	};

	/**
	 * Get the current position of the stream.
	 * @returns current position of the stream
	 */
	getPosition = () => {
		return this.position;
	};

	/**
	 * Set the current position of the stream.
	 * @param {number} position position in bytes
	 */
	setPosition = (position) => {
		if (typeof position === "number" && position >= 0 && position < this.getSize()) {
			this.position = position;
			return true;
		}
		throw new OutOfBoundsException("Can not set an invalid position.");
	};

	/**
	 * Decode a based64 string and load it into the binary stream.
	 * @param {string} base64 base64 encoded string containing the data.
	 */
	loadFromBase64 = (base64) => {
		this.binaryData = Base64Transcoder.decode(base64);
		this.position = 0;
	};

	/**
	 * Encode the binary data as base64 string.
	 * @returns base64 string
	 */
	saveToBase64 = () => {
		if (this.binaryData === null) {
			return "";
		}
		return Base64Transcoder.encode(this.binaryData);
	};

	/**
	 * Read the next byte from the stream.
	 */
	readByte = () => {
		if (this.binaryData === null || this.position >= this.getSize()) {
			throw new OutOfBoundsException("There is no further data available in the stream.");
		}
		return this.binaryData[this.position++];
	};

	/**
	 * Read the next word (uint16) from the stream.
	 * @returns next word from the stream
	 */
	readWord = () => {
		if (this.binaryData === null || this.position + 1 >= this.getSize()) {
			throw new OutOfBoundsException("There is no further data available in the stream.");
		}

		const first = this.readByte();
		const second = this.readByte();
		return ((second & 0xff) << 8) | (first & 0xff);
	};

	/**
	 * Read the next string from the stream.
	 * @returns next string from the stream
	 */
	readString = () => {
		if (this.binaryData === null || this.position + 1 >= this.getSize()) {
			throw new OutOfBoundsException("There is no further data available in the stream.");
		}

		const length = this.readWord();
		let string = "";
		try {
			for (let i = 0; i < length; i++) {
				string += String.fromCharCode([this.readByte()]);
			}
		} catch (error) {
			throw new OutOfBoundsException("Failed to read string from binary stream.", error);
		}
		return string;
	};

	/**
	 * Write a single byte to the {BinaryStream}.
	 * @param {*} byte byte to write to the stream
	 */
	writeByte = (byte) => {
		if (this.binaryData === null || this.position >= this.getSize()) {
			throw new OutOfBoundsException("There is no further space available in the stream to write data.");
		}
		this.binaryData[this.position++] = byte;
	};

	/**
	 * Write a single word to the {BinaryStream}.
	 * @param {*} word word to write to the stream
	 */
	writeWord = (word) => {
		if (this.binaryData === null || this.position + 1 >= this.getSize()) {
			throw new OutOfBoundsException("There is no further space available in the stream to write data.");
		}
		const first = word & 0xff;
		const second = (word >> 8) & 0xff;
		this.writeByte(first);
		this.writeByte(second);
	};

	/**
	 * Write a string to the {BinaryStream}.
	 * @param {string} string string to write to the stream
	 */
	writeString = (string) => {
		if (
			this.binaryData === null ||
			this.position + 1 >= this.getSize() ||
			this.position + 1 + string.length >= this.getSize()
		) {
			throw new OutOfBoundsException("There is no further space available in the stream to write data.");
		}

		this.writeWord(string.length);
		for (let i = 0; i < string.length; i++) {
			this.writeByte(string.charCodeAt(i));
		}
	};
}

export default BinaryStream;
