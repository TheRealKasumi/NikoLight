/**
 * @file BinaryFile.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::BianryFile}.
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
 * @return true when successful
 * @return false when the file could not be opened
 */
bool TL::BinaryFile::open(const String fileName, const char *mode)
{
	this->file = this->fileSystem->open(fileName, mode);
	if (!this->file)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open binary file."));
		return false;
	}
	else if (this->file.isDirectory())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open binary file because it is a directory."));
		this->file.close();
		return false;
	}

	return true;
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
 * @return true when successful
 * @return false when there was an error
 */
bool TL::BinaryFile::writeString(const String string)
{
	const uint16_t length = string.length();
	if (!this->write(length))
	{
		return false;
	}

	for (uint16_t i = 0; i < length; i++)
	{
		if (!this->write(string.charAt(i)))
		{
			return false;
		}
	}
	return true;
}

/**
 * @brief Read a string of variable length from the file.
 * @param string reference variable to hold the string
 * @return true when successful
 * @return false when there was an error
 */
bool TL::BinaryFile::readString(String &string)
{
	uint16_t length;
	if (!this->read(length))
	{
		return false;
	}

	string.clear();
	string.reserve(length);
	for (uint16_t i = 0; i < length; i++)
	{
		char c;
		if (!this->read(c))
		{
			return false;
		}
		string += c;
	}

	return true;
}
