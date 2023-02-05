/**
 * @file LedStrip.h
 * @author TheRealKasumi
 * @brief Contains a class that represents a single LED strip inside the {@link TL::LedBuffer}.
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
#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <stdint.h>
#include <stddef.h>
#include <cstdlib>

#include "Pixel.h"

namespace TL
{
	class LedStrip
	{
	public:
		LedStrip(const uint8_t ledPin, const size_t ledCount, const size_t hiddenLedCount = 8);
		~LedStrip();

		uint8_t getLedPin();
		size_t getLedCount();
		size_t getHiddenLedCount();

		TL::Pixel getPixel(const size_t index);
		void setPixel(const TL::Pixel &pixel, const size_t index);

		uint8_t *getBuffer();
		void setBuffer(uint8_t *buffer);

	private:
		uint8_t ledPin;
		size_t ledCount;
		size_t hiddenLedCount;
		uint8_t *buffer;
		bool initialized;
	};
}

#endif
