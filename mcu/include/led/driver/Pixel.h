/**
 * @file Pixel.h
 * @author TheRealKasumi
 * @brief Class representing a single pixel/led.
 * @version 0.0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>

namespace TesLight
{
	class Pixel
	{
	public:
		Pixel();
		~Pixel();

		void setColor(const uint32_t color);
		void setRed(const uint8_t red);
		void setGreen(const uint8_t green);
		void setBlue(const uint8_t blue);

		uint32_t getColor();
		uint8_t getRed();
		uint8_t getGreen();
		uint8_t getBlue();

	private:
		uint32_t color;
	};
}

#endif