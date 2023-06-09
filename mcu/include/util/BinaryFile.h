/**
 * @file BinaryFile.h
 * @author TheRealKasumi
 * @brief Class to read and write data in binary format.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef BINARY_FILE_H
#define BINARY_FILE_H

#include <Arduino.h>
#include <FS.h>

namespace NL
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

		NL::BinaryFile::Error open(const String fileName, const char *mode);
		void close();

		/**
		 * @brief Write a value to the binary file.
		 * @param value value to write
		 * @return OK when data was written
		 * @return ERROR_FILE_WRITE when data could not be written
		 */
		template <typename T>
		NL::BinaryFile::Error write(const T value)
		{
			return this->file.write((uint8_t *)&value, sizeof(value)) == sizeof(value) ? NL::BinaryFile::Error::OK : NL::BinaryFile::Error::ERROR_FILE_WRITE;
		}

		/**
		 * @brief Read a value from the file.
		 * @param value reference to the variable which will hold the value
		 * @return OK when data could not be read
		 * @return ERROR_FILE_READ when data could not be read
		 */
		template <typename T>
		NL::BinaryFile::Error read(T &value)
		{
			return this->file.read((uint8_t *)&value, sizeof(value)) == sizeof(value) ? NL::BinaryFile::Error::OK : NL::BinaryFile::Error::ERROR_FILE_READ;
		}

		NL::BinaryFile::Error writeString(const String string);
		NL::BinaryFile::Error readString(String &string);

	private:
		FS *fileSystem;
		File file;
	};
}

#endif