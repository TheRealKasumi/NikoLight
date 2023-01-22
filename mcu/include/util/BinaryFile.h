/**
 * @file BinaryFile.h
 * @author TheRealKasumi
 * @brief Class to read and write data in binary format.
 *
 * @copyright Copyright (c) 2022-2023 TheRealKasumi
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

namespace TL
{
	class BinaryFile
	{
	public:
		enum class Error
		{
			OK,					  // No error
			ERROR_FILE_NOT_FOUND, // File was not found
			ERROR_FILE_IS_DIR,	  // It is not a file but a directory
			ERROR_FILE_READ,	  // Could not read from the file
			ERROR_FILE_WRITE	  // Could not write to the file
		};

		BinaryFile(FS *fileSystem);
		~BinaryFile();

		TL::BinaryFile::Error open(const String fileName, const char *mode);
		void close();

		/**
		 * @brief Write a value to the binary file.
		 * @param value value to write
		 * @return OK when data was written
		 * @return ERROR_FILE_WRITE when data could not be written
		 */
		template <typename T>
		TL::BinaryFile::Error write(const T value)
		{
			return this->file.write((uint8_t *)&value, sizeof(value)) == sizeof(value) ? TL::BinaryFile::Error::OK : TL::BinaryFile::Error::ERROR_FILE_WRITE;
		}

		/**
		 * @brief Read a value from the file.
		 * @param value reference to the variable which will hold the value
		 * @return OK when data could not be read
		 * @return ERROR_FILE_READ when data could not be read
		 */
		template <typename T>
		TL::BinaryFile::Error read(T &value)
		{
			return this->file.read((uint8_t *)&value, sizeof(value)) == sizeof(value) ? TL::BinaryFile::Error::OK : TL::BinaryFile::Error::ERROR_FILE_READ;
		}

		TL::BinaryFile::Error writeString(const String string);
		TL::BinaryFile::Error readString(String &string);

	private:
		FS *fileSystem;
		File file;
	};
}

#endif