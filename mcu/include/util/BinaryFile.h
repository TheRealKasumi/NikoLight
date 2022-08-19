/**
 * @file BinaryFile.h
 * @author TheRealKasumi
 * @brief Class to read and write data in binary format.
 * @version 0.0.1
 * @date 2022-07-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef BINARY_FILE
#define BINARY_FILE

#include <Arduino.h>
#include <FS.h>

namespace TesLight
{
	class BinaryFile
	{
	public:
		BinaryFile(FS *fileSystem);
		~BinaryFile();

		bool open(String fileName, const char *mode);
		void close();

		bool writeByte(const uint8_t byte);
		uint8_t readByte();

		bool writeWord(const uint16_t word);
		uint16_t readWord();

		bool writeString(const String string);
		String readString();

	private:
		FS *fileSystem;
		File file;
	};
}

#endif