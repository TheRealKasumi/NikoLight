/**
 * @file LedManager.h
 * @author TheRealKasumi
 * @brief Contains a class to manage the LEDs using the FastLED library.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
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
#include "led/animator/SparkleAnimator.h"

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