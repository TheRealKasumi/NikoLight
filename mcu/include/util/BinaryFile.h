/**
 * @file BinaryFile.h
 * @author TheRealKasumi
 * @brief Class to read and write data in binary format.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef BINARY_FILE_H
#define BINARY_FILE_H

#include <Arduino.h>
#include <FS.h>

#include "logging/Logger.h"

namespace TesLight
{
	class BinaryFile
	{
	public:
		BinaryFile(FS *fileSystem);
		~BinaryFile();

		bool open(const String fileName, const char *mode);
		void close();

		/**
		 * @brief Write a value to the binary file.
		 * @param value value to write
		 * @return true when succesful
		 * @return false when there was an error
		 */
		template <typename T>
		bool write(const T value)
		{
			return this->file.write((uint8_t *)&value, sizeof(value)) == sizeof(value);
		}

		/**
		 * @brief Read a value from the file.
		 * @param value reference to the variable which will hold the value
		 * @return true when successful
		 * @return false when there was an error
		 */
		template <typename T>
		bool read(T &value)
		{
			return this->file.read((uint8_t *)&value, sizeof(value)) == sizeof(value);
		}

		bool writeString(const String string);
		bool readString(String &string);

	private:
		FS *fileSystem;
		File file;
	};
}

#endif