/**
 * @file LedDriver.h
 * @author TheRealKasumi
 * @brief Contains a class to drive WS2812 series LEDs using the ESP32 rmt lib.
 * @version 0.0.1
 * @date 2022-06-28
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

#define BITS_PER_LED_CMD 24
#define T0H 14
#define T1H 52
#define TL 52

namespace TesLight
{
	class LedDriver
	{
	public:
		LedDriver(const uint8_t pin, const uint8_t channel, const uint8_t pixelCount);
		~LedDriver();

		TesLight::Pixel *getPixels();
		uint16_t getPixelCount();

		void setPixel(const uint16_t index, const TesLight::Pixel pixel);
		TesLight::Pixel getPixel(const uint16_t index);

		void setActive(const bool active);

		bool begin();
		bool end();
		bool show();

	private:
		uint8_t pin;
		uint8_t channel;
		bool active;
		bool driverInstalled;
		uint16_t pixelCount;
		TesLight::Pixel *pixels;

		rmt_config_t getRmtConfig();

		bool installDriver(const rmt_config_t *config);
		bool uninstallDriver(const rmt_config_t *config);
		void prepareLedDataBuffer(rmt_item32_t *ledDataBuffer);
	};
}

#endif