/**
 * @file LedAnimator.h
 * @author TheRealKasumi
 * @brief Contains a base class for all led animators.
 * @version 0.0.1
 * @date 2022-09-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LED_ANIMATOR_H
#define LED_ANIMATOR_H

#include <stdint.h>
#include <math.h>

#include "led/driver/Pixel.h"

namespace TesLight
{
	class LedAnimator
	{
	public:
		LedAnimator();
		virtual ~LedAnimator() = 0;

		void setPixels(TesLight::Pixel *pixels);
		TesLight::Pixel *getPixels();

		void setPixelCount(const uint16_t pixelCount);
		uint16_t getPixelCount();

		void setSpeed(const uint8_t speed);
		uint8_t getSpeed();

		void setOffset(const uint16_t offset);
		uint16_t getOffset();

		void setBrightness(const float brightness);
		float getBrightness();

		void setReverse(const bool reverse);
		bool getReverse();

		virtual void init() = 0;
		virtual void render() = 0;

	protected:
		TesLight::Pixel *pixels;
		uint16_t pixelCount;
		uint8_t speed;
		uint16_t offset;
		float brightness;
		bool reverse;

		static float trapezoid(float angle);
	};
}

#endif