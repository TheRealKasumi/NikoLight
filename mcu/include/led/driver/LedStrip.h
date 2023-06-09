/**
 * @file LedStrip.h
 * @author TheRealKasumi
 * @brief Contains a class that represents a single LED strip inside the {@link NL::LedBuffer}.
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
#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <stdint.h>
#include <stddef.h>
#include <cstdlib>

#include "Pixel.h"

namespace NL
{
	class LedStrip
	{
	public:
		LedStrip(const uint8_t ledPin, const size_t ledCount, const size_t hiddenLedCount = 8);
		~LedStrip();

		uint8_t getLedPin();
		size_t getLedCount();
		size_t getHiddenLedCount();

		NL::Pixel getPixel(const size_t index);
		void setPixel(const NL::Pixel &pixel, const size_t index);

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
