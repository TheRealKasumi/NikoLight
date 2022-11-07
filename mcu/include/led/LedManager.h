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
		LedManager();
		~LedManager();

		bool loadFromConfiguration(TesLight::Configuration *config);
		void clear();

		void setAmbientBrightness(const float ambientBrightness);
		bool getAmbientBrightness(float &ambientBrightness);

		void setTargetFrameTime(const uint32_t targetFrameTime);
		uint32_t getTargetFrameTime();

		void setSystemPowerLimit(const uint8_t systemPowerLimit);
		uint8_t getSystemPowerLimit();

		void setLedVoltage(const uint8_t zoneIndex, const uint8_t ledVoltage);
		uint8_t getLedVoltage(const uint8_t zoneIndex);

		void setLedChannelCurrent(const uint8_t zoneIndex, const uint8_t redCurrent, const uint8_t greenCurrent, const uint8_t blueCurrent);
		uint8_t getLedRedChannelCurrent(const uint8_t zoneIndex);
		uint8_t getLedGreenChannelCurrent(const uint8_t zoneIndex);
		uint8_t getLedBlueChannelCurrent(const uint8_t zoneIndex);

		void setMotionSensorData(const TesLight::MotionSensor::MotionSensorData motionSensorData);
		bool getMotionSensorData(TesLight::MotionSensor::MotionSensorData &motionSensorData);

		void render();
		void show();

	private:
		CRGB *ledData[LED_NUM_ZONES];
		TesLight::LedAnimator *ledAnimator[LED_NUM_ZONES];
		TesLight::FseqLoader *fseqLoader = nullptr;
		uint32_t targetFrameTime;

		uint8_t systemPowerLimit;
		uint8_t ledVoltage[LED_NUM_ZONES];
		uint8_t ledChannelCurrent[LED_NUM_ZONES][3];

		bool createLedData(TesLight::Configuration *config);
		bool createAnimators(TesLight::Configuration *config);
		bool loadCalculatedAnimations(TesLight::Configuration *config);
		bool loadCustomAnimation(TesLight::Configuration *config);

		bool calculateRegulatorPowerDraw(float regulatorPower[REGULATOR_COUNT]);
		bool limitPowerConsumption();
	};
}

#endif