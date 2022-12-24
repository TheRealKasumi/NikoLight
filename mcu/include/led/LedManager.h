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

#include <vector>
#include <memory>
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
#include "led/animator/ColorBarAnimator.h"
#include "led/animator/RainbowAnimatorMotion.h"
#include "led/animator/GradientAnimatorMotion.h"

#include "sensor/MotionSensor.h"

namespace TL
{
	class LedManager
	{
	public:
		LedManager(TL::Configuration *config);
		~LedManager();

		bool reloadAnimations();
		void clearAnimations();

		void setAmbientBrightness(const float ambientBrightness);

		void setRenderInterval(const uint32_t renderInterval);
		uint32_t getRenderInterval();

		void setFrameInterval(const uint32_t frameInterval);
		uint32_t getFrameInterval();

		void setMotionSensorData(const TL::MotionSensor::MotionSensorData &motionSensorData);
		void setRegulatorTemperature(const float regulatorTemperature);

		float getLedPowerDraw();
		size_t getLedCount();

		void render();
		void show();

	private:
		TL::Configuration *config;
		std::vector<std::vector<CRGB>> ledData;
		std::vector<std::unique_ptr<TL::LedAnimator>> ledAnimator;
		std::unique_ptr<TL::FseqLoader> fseqLoader;

		uint32_t renderInterval;
		uint32_t frameInterval;
		float regulatorTemperature;

		bool createLedData();
		bool createAnimators();
		bool loadCalculatedAnimations();
		bool loadCustomAnimation();

		void calculateRegulatorPowerDraw(float regulatorPower[REGULATOR_COUNT]);
		void limitPowerConsumption();
		void limitRegulatorTemperature();

		uint8_t getRegulatorIndexFromPin(const uint8_t pin);
	};
}

#endif