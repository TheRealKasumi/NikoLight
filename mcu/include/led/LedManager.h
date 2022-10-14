/**
 * @file LedManager.h
 * @author TheRealKasumi
 * @brief Contains a class to manage the LEDs using the FastLED library.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <SD.h>

#include "configuration/SystemConfiguration.h"
#include "configuration/Configuration.h"

#include "logging/Logger.h"
#include "util/FileUtil.h"
#include "FastLED.h"

#include "led/animator/RainbowAnimator.h"
#include "led/animator/GradientAnimator.h"
#include "led/animator/StaticColorAnimator.h"
#include "led/animator/FseqAnimator.h"

namespace TesLight
{
	class LedManager
	{
	public:
		LedManager();
		~LedManager();

		bool loadFromConfiguration(TesLight::Configuration *config);
		void clear();

		void setAmbientBrightness(const float ambientBrightness);
		float getAmbientBrightness();

		void setTargetFrameTime(const uint32_t targetFrameTime);
		uint32_t getTargetFrameTime();

		void setSystemPowerLimit(const uint8_t systemPowerLimit);
		uint8_t getSystemPowerLimit();

		void setLedVoltage(const uint8_t ledVoltage);
		uint8_t getLedVoltage();

		void setLedChannelCurrent(const uint8_t redCurrent, const uint8_t greenCurrent, const uint8_t blueCurrent);
		uint8_t getLedRedChannelCurrent();
		uint8_t getLedGreenChannelCurrent();
		uint8_t getLedBlueChannelCurrent();

		void render();
		void show();

	private:
		CRGB *ledData[NUM_LED_STRIPS];
		TesLight::LedAnimator *ledAnimator[NUM_LED_STRIPS];
		TesLight::FseqLoader *fseqLoader = nullptr;
		uint32_t targetFrameTime;

		uint8_t systemPowerLimit;
		uint8_t ledVoltage;
		uint8_t ledChannelCurrent[3];

		bool createLedData(TesLight::Configuration *config);
		bool createAnimators(TesLight::Configuration *config);
		bool loadCalculatedAnimations(TesLight::Configuration *config);
		bool loadCustomAnimation(TesLight::Configuration *config);

		float calculatePowerConsumption();
		void limitPowerConsumption();
	};
}

#endif