/**
 * @file BinaryFile.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::BianryFile}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "util/BinaryFile.h"

/**
 * @brief Create a new instance of {@link TesLight::BinaryFile}.
 * @param fileSystem file system to use
 */
TesLight::BinaryFile::BinaryFile(FS *fileSystem)
{
	this->fileSystem = fileSystem;
}

/**
 * @brief Destroy the {@link TesLight::BinaryFile} instance.
 */
TesLight::BinaryFile::~BinaryFile()
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
bool TesLight::BinaryFile::open(const String fileName, const char *mode)
{
	this->file = this->fileSystem->open(fileName, mode);
	if (!this->file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open binary file."));
		return false;
	}
	else if (this->file.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to open binary file because it is a directory."));
		this->file.close();
		return false;
	}

	return true;
}

/**
 * @brief Close the file when it was opened.
 */
void TesLight::BinaryFile::close()
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
bool TesLight::BinaryFile::writeString(const String string)
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
bool TesLight::BinaryFile::readString(String &string)
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
