/**
 * @file LedStrip.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::LedStrip}.
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
#include "led/driver/LedStrip.h"

/**
 * @brief Create a new instance of {@link TL::LedStrip}.
 * @param ledPin physical pin number for the data output
 * @param ledCount number of LEDs
 */
TL::LedStrip::LedStrip(const uint8_t ledPin, const size_t ledCount)
{
	this->ledPin = ledPin;
	this->ledCount = ledCount;
	this->buffer = nullptr;
	this->isInitialized = false;
}

/**
 * @brief Destroy the {@link TL::LedStrip} instance.
 */
TL::LedStrip::~LedStrip()
{
}

/**
 * @brief Get the LED output pin.
 * @return pin number
 */
uint8_t TL::LedStrip::getLedPin()
{
	return this->ledPin;
}

/**
 * @brief Get the number of LEDs.
 * @return number of LEDs
 */
size_t TL::LedStrip::getLedCount()
{
	return this->ledCount;
}

/**
 * @brief Get a single pixel from the buffer.
 * @param index index of the pixel
 * @return OK when the
 */
TL::Pixel TL::LedStrip::getPixel(const size_t index)
{
	if (!this->isInitialized || index >= this->ledCount)
	{
		std::abort();
	}

	const size_t ind = index * 3;
	return TL::Pixel(this->buffer[ind], this->buffer[ind + 1], this->buffer[ind + 2]);
}

/**
 * @brief Set a single pixel in the buffer.
 * @param pixel updated pixel
 * @param index index of the pixel
 */
void TL::LedStrip::setPixel(const TL::Pixel &pixel, const size_t index)
{
	if (!this->isInitialized || index >= this->ledCount)
	{
		//std::abort();
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
uint8_t *TL::LedStrip::getBuffer()
{
	return this->buffer;
}

/**
 * @brief Set the base pointer to the buffer.
 * @param buffer base pointer to the buffer
 */
void TL::LedStrip::setBuffer(uint8_t *buffer)
{
	this->isInitialized = true;
	this->buffer = buffer;
}
