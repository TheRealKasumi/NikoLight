/**
 * @file LedManager.h
 * @author TheRealKasumi
 * @brief Contains a class to manage the LEDs using the FastLED library.
 *
 * @copyright Copyright (c) 2022-2023 TheRealKasumi
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

#include "led/driver/LedDriver.h"
#include "led/animator/RainbowAnimator.h"
#include "led/animator/GradientAnimator.h"
#include "led/animator/StaticColorAnimator.h"
#include "led/animator/FseqAnimator.h"
#include "led/animator/ColorBarAnimator.h"
#include "led/animator/RainbowAnimatorMotion.h"
#include "led/animator/GradientAnimatorMotion.h"
#include "led/animator/SparkleAnimator.h"

#include "util/FileUtil.h"
#include "sensor/MotionSensor.h"
#include "hardware/AudioUnit.h"

namespace TL
{
	class LedManager
	{
	public:
		enum class Error
		{
			OK,								// No error
			ERROR_CONFIG_UNAVAILABLE,		// The configuration is not available
			ERROR_INIT_LED_DRIVER,			// Failed to initialize the LED driver
			ERROR_DRIVER_NOT_READY,			// The LED driver is not ready to send new LED data
			ERROR_UNKNOWN_ANIMATOR_TYPE,	// The animator type is unknown
			ERROR_FILE_NOT_FOUND,			// The animation file was not found
			ERROR_INVALID_FSEQ,				// When a custom animation was set but the fseq file is invalid
			ERROR_INVALID_LED_CONFIGURATION // The current LED configuration does not match the custom animation
		};

		static TL::LedManager::Error begin();
		static void end();
		static bool isInitialized();

		static TL::LedManager::Error reloadAnimations();
		static void clearAnimations();

		static void setAmbientBrightness(const float ambientBrightness);

		static void setFrameInterval(const uint32_t frameInterval);
		static uint32_t getFrameInterval();

		static void setMotionSensorData(const TL::MotionSensor::MotionSensorData &motionSensorData);
		static void setAudioAnalysis(const TL::AudioUnit::AudioAnalysis &audioAnalysis);

		static void setRegulatorTemperature(const float regulatorTemperature);

		static float getLedPowerDraw();
		static size_t getLedCount();
		static size_t getHiddenLedCount();

		static void render();
		static TL::LedManager::Error waitShow(const TickType_t timeout);
		static TL::LedManager::Error show(const TickType_t timeout);

	private:
		LedManager();

		static bool initialized;
		static std::unique_ptr<TL::LedBuffer> ledBuffer;
		static std::vector<std::unique_ptr<TL::LedAnimator>> ledAnimator;
		static std::unique_ptr<TL::FseqLoader> fseqLoader;

		static uint32_t frameInterval;
		static float regulatorTemperature;

		static TL::LedManager::Error initLedDriver();
		static TL::LedManager::Error createAnimators();
		static TL::LedManager::Error loadCalculatedAnimations();
		static TL::LedManager::Error loadCustomAnimation(const String &fileName);

		static void calculateRegulatorPowerDraw(float regulatorPower[REGULATOR_COUNT]);
		static void limitPowerConsumption();
		static void limitRegulatorTemperature();
		static uint8_t getRegulatorIndexFromPin(const uint8_t pin);
	};
}

#endif