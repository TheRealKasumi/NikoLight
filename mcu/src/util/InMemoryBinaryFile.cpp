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
	this->buffer = new uint8_t[size];
	this->size = size;
	this->index = 0;
	this->freeOnDestroy = freeOnDestory;
}

/**
 * @brief Destroy the {@link TesLight::InMemoryBinaryFile} instance.
 */
TesLight::InMemoryBinaryFile::~InMemoryBinaryFile()
{
	if (this->freeOnDestroy)
	{
		delete[] this->buffer;
		this->buffer = nullptr;
	}
}

/**
 * @brief Get the data.
 */
uint8_t *TesLight::InMemoryBinaryFile::getData()
{
	return this->buffer;
}

/**
 * @brief Load the data from a given buffer.
 * @param buffer source data
 * @param bufferSize size of the buffer
 */
bool TesLight::InMemoryBinaryFile::loadFrom(uint8_t *buffer, const size_t bufferSize)
{
	if (bufferSize > this->size)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to load in memory binary File because the data exceeds the buffer size."));
		return false;
	}

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

	this->index = 0;
	return true;
}

/**
 * @brief Return the number of written bytes.
 * @return size_t number of written bytes
 */
size_t TesLight::InMemoryBinaryFile::getBytesWritten()
{
	return this->index;
}

/**
 * @brief Write a string of variable length to the buffer.
 * @param string string to write
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::InMemoryBinaryFile::writeString(const String string)
{
	const uint16_t length = string.length();
	if (this->index + sizeof(length) + length - 1 >= this->size || !this->write(length))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to write to in memory binary File because the data exceeds the buffer size."));
		return false;
	}

	for (uint16_t i = 0; i < length; i++)
	{
		this->buffer[this->index++] = string.charAt(i);
	}

	return true;
}

/**
 * @brief Read a string (variable length) from the buffer.
 * @param string reference variable to hold the string
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::InMemoryBinaryFile::readString(String &string)
{
	uint16_t length = 0;
	if (this->index + sizeof(uint16_t) >= this->size || !this->read(length))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read from in memory binary File because the end is reached."));
		return false;
	}

	string.clear();
	string.reserve(length);
	for (uint16_t i = 0; i < length && this->index < this->size; i++)
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
