/**
 * @file LedBuffer.h
 * @author TheRealKasumi
 * @brief Contains a class to manage the LED buffer for multiple LED strips.
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
#ifndef LED_BUFFER_H
#define LED_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <vector>

#include "LedStrip.h"

namespace NL
{
	class LedBuffer
	{
	public:
		LedBuffer(const std::vector<NL::LedStrip> &ledStrips);
		~LedBuffer();

		size_t getBufferSize();
		uint8_t *getBuffer();

		size_t getTotalLedCount();
		size_t getMaxLedCount();

		size_t getTotalHiddenLedCount();
		size_t getMaxHiddenLedCount();

		size_t getLedStripCount();
		NL::LedStrip &getLedStrip(const size_t index);

	private:
		std::vector<NL::LedStrip> ledStrips;
		size_t totalLedCount;
		size_t maxLedCount;
		size_t totalHiddenLedCount;
		size_t maxHiddenLedCount;
		uint8_t *buffer;
	};
}

#endif
