/**
 * @file LedBuffer.h
 * @author TheRealKasumi
 * @brief Contains a class to manage the LED buffer for multiple LED strips.
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
#ifndef LED_BUFFER_H
#define LED_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <vector>

#include "LedStrip.h"

namespace TL
{
	class LedBuffer
	{
	public:
		LedBuffer(const std::vector<TL::LedStrip> &ledStrips);
		~LedBuffer();

		size_t getBufferSize();
		uint8_t *getBuffer();

		size_t getTotalLedCount();
		size_t getMaxLedCount();

		size_t getTotalHiddenLedCount();
		size_t getMaxHiddenLedCount();

		size_t getLedStripCount();
		TL::LedStrip &getLedStrip(const size_t index);

	private:
		std::vector<TL::LedStrip> ledStrips;
		size_t totalLedCount;
		size_t maxLedCount;
		size_t totalHiddenLedCount;
		size_t maxHiddenLedCount;
		uint8_t *buffer;
	};
}

#endif
