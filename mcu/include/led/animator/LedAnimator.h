/**
 * @file LedAnimator.h
 * @author TheRealKasumi
 * @brief Contains a base class for all led animators.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LED_ANIMATOR_H
#define LED_ANIMATOR_H

#include <stdint.h>
#include <math.h>

#include "FastLED.h"

namespace TesLight
{
	class LedAnimator
	{
	public:
		LedAnimator();
		virtual ~LedAnimator() = 0;

		void setPixels(CRGB *pixels);
		CRGB *getPixels();

		void setPixelCount(const uint16_t pixelCount);
		uint16_t getPixelCount();

		void setSpeed(const uint8_t speed);
		uint8_t getSpeed();

		void setOffset(const uint16_t offset);
		uint16_t getOffset();

		void setAnimationBrightness(const float animationBrightness);
		float getAnimationBrightness();

		void setAmbientBrightness(const float ambientBrightness);
		float getAmbientBrightness();

		void setFadeSpeed(const float fadeSpeed);
		float getFadeSpeed();

		void setReverse(const bool reverse);
		bool getReverse();

		virtual void init() = 0;
		virtual void render() = 0;

	protected:
		CRGB *pixels;
		uint16_t pixelCount;
		uint8_t speed;
		uint16_t offset;
		float animationBrightness;
		float ambientBrightness;
		float smoothedAmbBrightness;
		float fadeSpeed;
		bool reverse;

		void applyBrightness();
		static float trapezoid(float angle);
	};
}

#endif