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
 * @brief Write a single byte to the buffer.
 * @param byte data to write
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::InMemoryBinaryFile::writeByte(const uint8_t byte)
{
	if (this->index >= this->size)
	{
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
	if (this->index >= this->size)
	{
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
	if (this->index + sizeof(word) - 1 >= this->size)
	{
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
	if (this->index + sizeof(word) - 1 >= this->size)
	{
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
	const uint16_t length = string.length();
	if (this->index + sizeof(word) + length - 1 >= this->size)
	{
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
	if (this->index + sizeof(word) >= this->size)
	{
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
