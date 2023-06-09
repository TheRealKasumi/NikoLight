/**
 * @file LedStrip.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::LedStrip}.
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
#include "led/driver/LedStrip.h"

/**
 * @brief Create a new instance of {@link NL::LedStrip}.
 * @param ledPin physical pin number for the data output
 * @param ledCount number of visible LEDs
 * @param hiddenLedCount total number of LEDs including invisible ones
 */
NL::LedStrip::LedStrip(const uint8_t ledPin, const size_t ledCount, const size_t hiddenLedCount)
{
	this->ledPin = ledPin;
	this->ledCount = ledCount;
	this->hiddenLedCount = hiddenLedCount;
	this->buffer = nullptr;
	this->initialized = false;

	if (this->hiddenLedCount < this->ledCount)
	{
		this->hiddenLedCount = ledCount;
	}
	else if (this->hiddenLedCount < 8)
	{
		this->hiddenLedCount = 8;
	}
}

/**
 * @brief Destroy the {@link NL::LedStrip} instance.
 */
NL::LedStrip::~LedStrip()
{
}

/**
 * @brief Get the LED output pin.
 * @return pin number
 */
uint8_t NL::LedStrip::getLedPin()
{
	return this->ledPin;
}

/**
 * @brief Get the number of LEDs.
 * @return number of LEDs
 */
size_t NL::LedStrip::getLedCount()
{
	return this->ledCount;
}

/**
 * @brief Get the number of hidden LEDs. This is the number of visible LEDs + the number of invisible LEDs.
 * @return number of hidden LEDs
 */
size_t NL::LedStrip::getHiddenLedCount()
{
	return this->hiddenLedCount;
}

/**
 * @brief Get a single pixel from the buffer.
 * @param index index of the pixel
 * @return OK when the
 */
NL::Pixel NL::LedStrip::getPixel(const size_t index)
{
	if (!this->initialized || index >= this->ledCount)
	{
		std::abort();
	}
	const size_t ind = index * 3;
	return NL::Pixel(this->buffer[ind], this->buffer[ind + 1], this->buffer[ind + 2]);
}

/**
 * @brief Set a single pixel in the buffer.
 * @param pixel updated pixel
 * @param index index of the pixel
 */
void NL::LedStrip::setPixel(const NL::Pixel &pixel, const size_t index)
{
	if (!this->initialized || index >= this->ledCount)
	{
		std::abort();
	}
	const size_t ind = index * 3;
	this->buffer[ind] = pixel.red;
	this->buffer[ind + 1] = pixel.green;
	this->buffer[ind + 2] = pixel.blue;
}

/**
 * @brief Return the base pointer to the buffer.
 * @return pointer to the buffer
 */
uint8_t *NL::LedStrip::getBuffer()
{
	if (!this->initialized)
	{
		std::abort();
	}
	return this->buffer;
}

/**
 * @brief Set the base pointer to the buffer.
 * @param buffer base pointer to the buffer
 */
void NL::LedStrip::setBuffer(uint8_t *buffer)
{
	this->initialized = true;
	this->buffer = buffer;
}
