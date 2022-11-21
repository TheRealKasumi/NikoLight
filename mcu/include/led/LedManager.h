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
#include "led/animator/ColorBarAnimator.h"
#include "led/animator/RainbowAnimatorMotion.h"
#include "led/animator/GradientAnimatorMotion.h"

#include "sensor/MotionSensor.h"

namespace TesLight
{
	class LedManager
	{
	public:
		LedManager(TesLight::Configuration *config);
		~LedManager();

		bool reloadAnimations();
		void clearAnimations();

		void setAmbientBrightness(const float ambientBrightness);
		bool getAmbientBrightness(float &ambientBrightness);

		void setTargetFrameTime(const uint32_t targetFrameTime);
		uint32_t getTargetFrameTime();

		void setMotionSensorData(const TesLight::MotionSensor::MotionSensorData motionSensorData);
		bool getMotionSensorData(TesLight::MotionSensor::MotionSensorData &motionSensorData);

		void setRegulatorTemperature(const float regulatorTemperature);
		float getRegulatorTemperature();

		float getLedPowerDraw();

		bool render();
		void show();

	private:
		TesLight::Configuration *config;

		CRGB *ledData[LED_NUM_ZONES];
		TesLight::LedAnimator *ledAnimator[LED_NUM_ZONES];
		TesLight::FseqLoader *fseqLoader = nullptr;
		uint32_t targetFrameTime;

		float regulatorTemperature;

		bool createLedData();
		bool createAnimators();
		bool loadCalculatedAnimations();
		bool loadCustomAnimation();

		bool calculateRegulatorPowerDraw(float regulatorPower[REGULATOR_COUNT]);
		bool limitPowerConsumption();
		bool limitRegulatorTemperature();

		uint8_t getRegulatorIndexFromPin(const uint8_t pin);
	};
}

#endif