/**
 * @file LedBuffer.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::LedBuffer}.
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
