/**
 * @file LedBuffer.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::LedBuffer}.
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
#include "led/driver/LedBuffer.h"

/**
 * @brief Create a new instance of {@link NL::LedBuffer}.
 * @param ledStrips vector holding the LED strips
 */
NL::LedBuffer::LedBuffer(const std::vector<NL::LedStrip> &ledStrips)
{
	this->ledStrips = ledStrips;
	this->totalLedCount = 0;
	this->maxLedCount = 0;
	this->totalHiddenLedCount = 0;
	this->maxHiddenLedCount = 0;

	for (size_t i = 0; i < this->ledStrips.size(); i++)
	{
		const size_t ledCount = this->ledStrips.at(i).getLedCount();
		this->totalLedCount += ledCount;
		this->maxLedCount = ledCount > this->maxLedCount ? ledCount : this->maxLedCount;

		const size_t hiddenLedCount = this->ledStrips.at(i).getHiddenLedCount();
		this->totalHiddenLedCount += hiddenLedCount;
		this->maxHiddenLedCount = hiddenLedCount > this->maxHiddenLedCount ? hiddenLedCount : this->maxHiddenLedCount;
	}

	const size_t bufferSize = this->totalHiddenLedCount * 3;
	this->buffer = new uint8_t[bufferSize];
	for (size_t i = 0; i < bufferSize; i++)
	{
		this->buffer[i] = 0;
	}

	uint8_t *ptr = this->buffer;
	for (size_t i = 0; i < this->ledStrips.size(); i++)
	{
		this->ledStrips.at(i).setBuffer(ptr);
		ptr += this->ledStrips.at(i).getHiddenLedCount() * 3;
	}
}

/**
 * @brief Destroy the {@link NL::LedBuffer} instance and free memory.
 */
NL::LedBuffer::~LedBuffer()
{
	if (this->buffer != nullptr)
	{
		delete[] this->buffer;
		this->buffer = nullptr;
	}
}

/**
 * @brief Get the size of the LED buffer in bytes.
 * @return size of the LED buffer in bytes.
 */
size_t NL::LedBuffer::getBufferSize()
{
	return this->totalLedCount * 3;
}

/**
 * @brief Get the base pointer to the LED buffer.
 * @return pointer to the LED buffer
 */
uint8_t *NL::LedBuffer::getBuffer()
{
	return this->buffer;
}

/**
 * @brief Get the total number of visible LEDs.
 * @return total number of LEDs
 */
size_t NL::LedBuffer::getTotalLedCount()
{
	return this->totalLedCount;
}

/**
 * @brief Get the highst number of visible LEDs from all strips.
 * @return highest number of LED per strip
 */
size_t NL::LedBuffer::getMaxLedCount()
{
	return this->maxLedCount;
}

/**
 * @brief Get the total number of all LEDs.
 * @return total number of LEDs
 */
size_t NL::LedBuffer::getTotalHiddenLedCount()
{
	return this->totalHiddenLedCount;
}

/**
 * @brief Get the highst number of LEDs from all strips.
 * @return highest number of LED per strip
 */
size_t NL::LedBuffer::getMaxHiddenLedCount()
{
	return this->maxHiddenLedCount;
}

/**
 * @brief Get the number of LED strips.
 * @return number of LED strips
 */
size_t NL::LedBuffer::getLedStripCount()
{
	return this->ledStrips.size();
}

/**
 * @brief Get a single LED strip.
 * @param index index of the LED strip
 * @return reference to the LED strip
 */
NL::LedStrip &NL::LedBuffer::getLedStrip(const size_t index)
{
	return this->ledStrips.at(index);
}
