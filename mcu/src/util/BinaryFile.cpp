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
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize binary file."));
	this->fileSystem = fileSystem;
}

/**
 * @brief Destroy the {@link TesLight::BinaryFile} instance.
 */
TesLight::BinaryFile::~BinaryFile()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Delete binary file and close resources."));
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
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Open binary file ") + fileName + F(" with mode ") + String(mode) + F("."));
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

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Binary file opened."));
	return true;
}

/**
 * @brief Close the file when it was opened.
 */
void TesLight::BinaryFile::close()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Closing binary file."));
	if (this->file)
	{
		this->file.close();
	}
}

/**
 * @brief Write a single byte to the file.
 * @param byte data to write
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::BinaryFile::writeByte(const uint8_t byte)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Write byte ") + String(byte) + F(" to binary file."));
	return this->file.write(byte) == sizeof(byte);
}

/**
 * @brief Read a single byte from the file.
 * @return uint8_t read byte
 */
uint8_t TesLight::BinaryFile::readByte()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Read byte from binary file."));
	uint8_t byte;
	this->file.read(&byte, sizeof(byte));
	return byte;
}

/**
 * @brief Write a word (2 byte) to the file.
 * @param word data to write
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::BinaryFile::writeWord(const uint16_t word)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Write word ") + String(word) + F(" to binary file."));
	return this->file.write((uint8_t *)&word, sizeof(word)) == sizeof(word);
}

/**
 * @brief Read a word (2 byte) from the file.
 * @return uint16_t read data
 */
uint16_t TesLight::BinaryFile::readWord()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Read word from binary file."));
	uint16_t word;
	this->file.read((uint8_t *)&word, sizeof(word));
	return word;
}

/**
 * @brief Write a string (variable lengt) to the file.
 * @param string string to write
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::BinaryFile::writeString(const String string)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Write string \"") + string + F("\" to binary file."));
	const uint16_t length = string.length();
	if (!this->writeWord(length))
	{
		return false;
	}

	for (uint16_t i = 0; i < length; i++)
	{
		if (!this->writeByte(string.charAt(i)))
		{
			return false;
		}
	}
	return true;
}

/**
 * @brief Read a string (variable length) from the file.
 * @return String read string
 */
String TesLight::BinaryFile::readString()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Read string from binary file."));
	const uint16_t length = this->readWord();

	String string;
	string.reserve(length);
	for (uint16_t i = 0; i < length; i++)
	{
		string += (char)this->readByte();
	}

	return string;
}
