/**
 * @file LedDriver.h
 * @author TheRealKasumi
 * @brief Contains a class to drive WS2812 series LEDs using the ESP32 rmt lib.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <stdint.h>
#include <driver/rmt.h>

#include "led/driver/Pixel.h"
#include "led/animator/LedAnimator.h"
#include "logging/Logger.h"

#define BITS_PER_LED 24
#define T0H 14
#define T1H 52
#define TL 52

namespace TesLight
{
	class LedDriver
	{
	public:
		LedDriver(const uint8_t pin, const uint8_t channel, const uint8_t pixelCount, const float animationBrightness = 0.0f, const float lightBrightness = 0.0f, const float fadeSpeed = 0.02f);
		~LedDriver();

		TesLight::Pixel *getPixels();
		uint16_t getPixelCount();

		void setPixel(const uint16_t index, const TesLight::Pixel pixel);
		TesLight::Pixel getPixel(const uint16_t index);

		float getAnimationBrightness();
		void setAnimationBrightness(const float animationBrightness);

		float getLightBrightness();
		void setLightBrightness(const float lightBrightness);

		float getFadeSpeed();
		void setFadeSpeed(const float fadeSpeed);

		bool begin();
		bool end();
		bool show();

	private:
		uint8_t pin;
		uint8_t channel;
		uint16_t pixelCount;
		float animationBrightness;
		float lightBrightness;
		float fadeSpeed;

		bool driverInstalled;
		TesLight::Pixel *pixels;
		float smoothedLightBrightness;
		float totalBrightness;

		rmt_config_t getRmtConfig();

		bool installDriver(const rmt_config_t *config);
		bool uninstallDriver(const rmt_config_t *config);
		void prepareLedDataBuffer(rmt_item32_t *ledDataBuffer);
	};
}

#endif