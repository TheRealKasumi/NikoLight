/**
 * @file LedManager.h
 * @author TheRealKasumi
 * @brief Contains a class to manage the LEDs using the FastLED library.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
#include "led/animator/PulseAnimator.h"
#include "led/animator/EqualizerAnimator.h"

#include "util/FileUtil.h"
#include "sensor/MotionSensor.h"
#include "hardware/AudioUnit.h"

namespace NL
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

		static NL::LedManager::Error begin();
		static void end();
		static bool isInitialized();

		static NL::LedManager::Error reloadAnimations();
		static void clearAnimations();

		static void setAmbientBrightness(const float ambientBrightness);

		static void setFrameInterval(const uint32_t frameInterval);
		static uint32_t getFrameInterval();

		static void setMotionSensorData(const NL::MotionSensor::MotionSensorData &motionSensorData);
		static void setAudioAnalysis(const NL::AudioUnit::AudioAnalysis &audioAnalysis);

		static void setRegulatorTemperature(const float regulatorTemperature);

		static float getLedPowerDraw();
		static size_t getLedCount();
		static size_t getHiddenLedCount();

		static void render();
		static NL::LedManager::Error waitShow(const TickType_t timeout);
		static NL::LedManager::Error show(const TickType_t timeout);

	private:
		LedManager();

		static bool initialized;
		static std::unique_ptr<NL::LedBuffer> ledBuffer;
		static std::vector<std::unique_ptr<NL::LedAnimator>> ledAnimator;
		static std::unique_ptr<NL::FseqLoader> fseqLoader;

		static uint32_t frameInterval;
		static float regulatorTemperature;

		static NL::LedManager::Error initLedDriver();
		static NL::LedManager::Error createAnimators();
		static NL::LedManager::Error loadCalculatedAnimations();
		static NL::LedManager::Error loadCustomAnimation(const String &fileName);

		static void calculateRegulatorPowerDraw(float regulatorPower[REGULATOR_COUNT]);
		static void limitPowerConsumption();
		static void limitRegulatorTemperature();
		static uint8_t getRegulatorIndexFromPin(const uint8_t pin);
	};
}

#endif