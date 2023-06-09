/**
 * @file Fan.h
 * @author TheRealKasumi
 * @brief Contains a class to control the cooling fan.
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
#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include <stdint.h>
#include <Arduino.h>
#include "configuration/Configuration.h"
#include "sensor/TemperatureSensor.h"

namespace NL
{
	class Fan
	{
	public:
		enum class Error
		{
			OK,						  // No error
			ERROR_CONFIG_UNAVAILABLE, // The configuration is not available
			ERROR_TEMP_UNAVAILABLE,	  // The temperature sensor is not available
			ERROR_SETUP_PIN,		  // Error setting up the pin
			ERROR_UNKNOWN_MODE		  // Unknown fan mode
		};

		enum class FanMode : uint8_t
		{
			AUTOMATIC = 0,	// Auomatic fan speed control
			MANUAL_OFF = 1, // Manual fan speed 0%
			MANUAL_25 = 2,	// Manual fan speed 25%
			MANUAL_50 = 3,	// Manual fan speed 50%
			MANUAL_75 = 4,	// Manual fan speed 75%
			MANUAL_100 = 5	// Manual fan speed 100%
		};

		static NL::Fan::Error begin(const uint8_t fanPin, const uint8_t pwmChannel, const uint32_t frequency, const uint8_t resolution);
		static void end();
		static bool isInitialized();

		static NL::Fan::Error run(const NL::Fan::FanMode fanMode);
		static uint8_t getPwmValue();

	private:
		Fan();

		static bool initialized;
		static uint8_t fanPin;
		static uint8_t pwmChannel;
		static uint32_t frequency;
		static uint8_t resolution;
		static uint8_t pwmValue;
	};
}

#endif