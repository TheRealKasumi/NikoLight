/**
 * @file InMemoryBinaryFile.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::InMemoryBinaryFile}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "util/InMemoryBinaryFile.h"

/**
 * @brief Create a new instance of {@link TesLight::InMemoryBinaryFile}.
 * @param fileSystem file system to use
 */
TesLight::InMemoryBinaryFile::InMemoryBinaryFile(const size_t size, const bool freeOnDestory)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Initialize In Memory Binary File with size ") + String(size) + F("."));
	this->buffer = new uint8_t[size];
	this->size = size;
	this->index = 0;
	this->freeOnDestroy = freeOnDestory;
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("In Memory Binary File initialized."));
}

/**
 * @brief Destroy the {@link TesLight::InMemoryBinaryFile} instance.
 */
TesLight::InMemoryBinaryFile::~InMemoryBinaryFile()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Deleting In Memory Binary File."));
	if (this->freeOnDestroy)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Freeing allocated memory."));
		delete[] this->buffer;
		this->buffer = nullptr;
	}
}

/**
 * @brief Get the data.
 */
uint8_t *TesLight::InMemoryBinaryFile::getData()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get binary file data."));
	return this->buffer;
}

/**
 * @brief Load the data from a given buffer.
 * @param buffer source data
 * @param bufferSize size of the buffer
 */
bool TesLight::InMemoryBinaryFile::loadFrom(uint8_t *buffer, const size_t bufferSize)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Load In Memory Binary File."));
	if (bufferSize > this->size)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to load In Memory Binary File because the data exceeds the buffer size."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Copy data into buffer."));
	for (size_t i = 0; i < this->size; i++)
	{
		if (i < bufferSize)
		{
			this->buffer[i] = buffer[i];
		}
		else
		{
			this->buffer[i] = 0;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Data loaded."));
	this->index = 0;
	return true;
}

/**
 * @brief Return the number of written bytes.
 * @return size_t number of written bytes
 */
size_t TesLight::InMemoryBinaryFile::getBytesWritten()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get number of written bytes."));
	return this->index;
}

/**
 * @brief Write a single byte to the buffer.
 * @param byte data to write
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::InMemoryBinaryFile::writeByte(const uint8_t byte)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Write byte ") + String(byte) + F(" to In Memory Binary File."));
	if (this->index >= this->size)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to write to In Memory Binary File because the data exceeds the buffer size."));
		return false;
	}
	this->buffer[this->index++] = byte;
	return true;
}

/**
 * @brief Read a single byte from the file.
 * @return uint8_t read byte
 */
uint8_t TesLight::InMemoryBinaryFile::readByte()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Read byte from In Memory Binary File."));
	if (this->index >= this->size)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read from In Memory Binary File because the end is reached."));
		return 0;
	}
	return this->buffer[this->index++];
}

/**
 * @brief Write a word (2 byte) to the buffer.
 * @param word data to write
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::InMemoryBinaryFile::writeWord(const uint16_t word)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Write word ") + String(word) + F(" to In Memory Binary File."));
	if (this->index + sizeof(word) - 1 >= this->size)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to write to In Memory Binary File because the data exceeds the buffer size."));
		return false;
	}
	this->buffer[this->index++] = ((uint8_t *)&word)[0];
	this->buffer[this->index++] = ((uint8_t *)&word)[1];
	return true;
}

/**
 * @brief Read a word (2 byte) from the file.
 * @return uint16_t read data
 */
uint16_t TesLight::InMemoryBinaryFile::readWord()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Read word from In Memory Binary File."));
	if (this->index + sizeof(uint16_t) - 1 >= this->size)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read from In Memory Binary File because the end is reached."));
		return 0;
	}
	uint16_t word;
	((uint8_t *)&word)[0] = this->buffer[this->index++];
	((uint8_t *)&word)[1] = this->buffer[this->index++];
	return word;
}

/**
 * @brief Write a string (variable lengt) to the buffer.
 * @param string string to write
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::InMemoryBinaryFile::writeString(const String string)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Write string \"") + string + F("\" to In Memory Binary File."));
	const uint16_t length = string.length();
	if (this->index + sizeof(uint16_t) + length - 1 >= this->size)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to write to In Memory Binary File because the data exceeds the buffer size."));
		return false;
	}

	this->writeWord(length);
	for (uint16_t i = 0; i < length; i++)
	{
		this->buffer[this->index++] = string.charAt(i);
	}

	return true;
}

/**
 * @brief Read a string (variable length) from the buffer.
 * @return String read string
 */
String TesLight::InMemoryBinaryFile::readString()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Read string from In Memory Binary File."));
	if (this->index + sizeof(uint16_t) >= this->size)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read from In Memory Binary File because the end is reached."));
		return "";
	}

	const uint16_t length = this->readWord();

	String string;
	string.reserve(length);
	for (uint16_t i = 0; i < length && this->index < this->size; i++)
	{
		string += (char)this->readByte();
	}

	return string;
}
