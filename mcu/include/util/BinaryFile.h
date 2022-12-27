/**
 * @file BinaryFile.h
 * @author TheRealKasumi
 * @brief Class to read and write data in binary format.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
 * 
 * This project, including hardware and software, is provided "as is". There is no warranty
 * of any kind, express or implied, including but not limited to the warranties of fitness
 * for a particular purpose and noninfringement. TheRealKasumi (https://github.com/TheRealKasumi)
 * is holding ownership of this project. You are free to use, modify, distribute and contribute
 * to this project for private, non-commercial purposes. It is granted to include this hardware
 * and software into private, non-commercial projects. However, the source code of any project,
 * software and hardware that is including this project must be public and free to use for private
 * persons. Any commercial use is hereby strictly prohibited without agreement from the owner.
 * By contributing to the project, you agree that the ownership of your work is transferred to
 * the project owner and that you lose any claim to your contribute work. This copyright and
 * license note applies to all files of this project and must not be removed without agreement
 * from the owner.
 *
 */
#ifndef BINARY_FILE_H
#define BINARY_FILE_H

#include <Arduino.h>
#include <FS.h>

#include "logging/Logger.h"

namespace TL
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