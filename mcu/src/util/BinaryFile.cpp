/**
 * @file BinaryFile.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::BianryFile}.
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
#include "util/BinaryFile.h"

/**
 * @brief Create a new instance of {@link TL::BinaryFile}.
 * @param fileSystem file system to use
 */
TL::BinaryFile::BinaryFile(FS *fileSystem)
{
	this->fileSystem = fileSystem;
}

/**
 * @brief Destroy the {@link TL::BinaryFile} instance.
 */
TL::BinaryFile::~BinaryFile()
{
	if (this->file)
	{
		this->file.close();
	}
}

/**
 * @brief Open a file from the file system with a given name
 * @param fileName full path and name of the file
 * @param mode read or write
 * @return OK when the file was opened
 * @return ERROR_FILE_NOT_FOUND when the file was not found
 * @return ERROR_FILE_IS_DIR when a directory was found
 */
TL::BinaryFile::Error TL::BinaryFile::open(const String fileName, const char *mode)
{
	this->file = this->fileSystem->open(fileName, mode);
	if (!this->file)
	{
		return TL::BinaryFile::Error::ERROR_FILE_NOT_FOUND;
	}
	else if (this->file.isDirectory())
	{
		this->file.close();
		return TL::BinaryFile::Error::ERROR_FILE_IS_DIR;
	}

	return TL::BinaryFile::Error::OK;
}

/**
 * @brief Close the file when it was opened.
 */
void TL::BinaryFile::close()
{
	if (this->file)
	{
		this->file.close();
	}
}

/**
 * @brief Write a string (variable lengt) to the file.
 * @param string string to write
 * @return OK when data was written
 * @return ERROR_FILE_WRITE when data could not be written
 */
TL::BinaryFile::Error TL::BinaryFile::writeString(const String string)
{
	const uint16_t length = string.length();
	const TL::BinaryFile::Error writeLenError = this->write(length);
	if (writeLenError != TL::BinaryFile::Error::OK)
	{
		return writeLenError;
	}

	for (uint16_t i = 0; i < length; i++)
	{
		const TL::BinaryFile::Error writeCharError = this->write(string.charAt(i));
		if (writeCharError != TL::BinaryFile::Error::OK)
		{
			return writeCharError;
		}
	}

	return TL::BinaryFile::Error::OK;
}

/**
 * @brief Read a string of variable length from the file.
 * @param string reference variable to hold the string
 * @return OK when data could not be read
 * @return ERROR_FILE_READ when data could not be read
 */
TL::BinaryFile::Error TL::BinaryFile::readString(String &string)
{
	uint16_t length;
	const TL::BinaryFile::Error readLenError = this->read(length);
	if (readLenError != TL::BinaryFile::Error::OK)
	{
		return readLenError;
	}

	string.clear();
	string.reserve(length);
	for (uint16_t i = 0; i < length; i++)
	{
		char c;
		const TL::BinaryFile::Error readCharError = this->read(c);
		if (readCharError != TL::BinaryFile::Error::OK)
		{
			return readCharError;
		}
		string += c;
	}

	return TL::BinaryFile::Error::OK;
}
