/**
 * @file LightSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the ambient brightness from either the cars ambient light or an external light sensor.
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
#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <stdint.h>

#include "configuration/SystemConfiguration.h"
#include "configuration/Configuration.h"
#include "hardware/AnalogInput.h"
#include "hardware/BH1750.h"

#include "sensor/MotionSensor.h"

namespace NL
{
	class LightSensor
	{
	public:
		enum class Error
		{
			OK,						   // No error
			ERROR_CONFIG_UNAVAILABLE,  // The configuration is not available
			ERROR_ADC_UNAVAILABLE,	   // The ADC is not available
			ERROR_BH1750_UNAVAILABLE,  // The BH1750 is not available
			ERROR_MPU6050_UNAVAILABLE, // The MPU6050 is not available
			ERROR_UNKNOWN_MODE,		   // The light sensor mode is unknown
		};

		enum class LightSensorMode : uint8_t
		{
			ALWAYS_OFF = 0,
			ALWAYS_ON = 1,
			AUTO_ON_OFF_ADC = 2,
			AUTO_BRIGHTNESS_ADC = 3,
			AUTO_ON_OFF_BH1750 = 4,
			AUTO_BRIGHTNESS_BH1750 = 5,
			AUTO_ON_OFF_MOTION = 6
		};

		static NL::LightSensor::Error begin();
		static void end();
		static bool isInitialized();

		static NL::LightSensor::Error getBrightness(float &brightness);

	private:
		LightSensor();

		static bool initialized;
		static float lastBrightnessValue;
		static NL::MotionSensor::MotionSensorData motionData;
		static unsigned long motionSensorTriggerTime;

		static NL::LightSensor::Error getBrightnessInt(float &brightness);
	};
}

#endif