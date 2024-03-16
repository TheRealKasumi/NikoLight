/**
 * @file TemperatureSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the temperature from the DS18B20 sensors.
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
#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <stdint.h>

#include "configuration/SystemConfiguration.h"
#include "hardware/DS18B20.h"

#if defined(HW_VERSION_2_2)
#include "hardware/LM75BD.h"
#endif

namespace NL
{
	class TemperatureSensor
	{
	public:
		enum class Error
		{
			OK,						  // No error
			ERROR_DS18B20_UNAVAILABLE // The DS18B20 sensor is not available
		};

#if defined(HW_VERSION_1_0) || defined(HW_VERSION_2_0) || defined(HW_VERSION_2_1)
		static NL::TemperatureSensor::Error begin();
#elif defined(HW_VERSION_2_2)
		static NL::TemperatureSensor::Error begin(NL::LM75BD *lm75);
#endif

		static void end();
		static bool isInitialized();

		static NL::TemperatureSensor::Error getMinTemperature(float &temp);
		static NL::TemperatureSensor::Error getMaxTemperature(float &temp);
		static NL::TemperatureSensor::Error getAverageTemperature(float &temp);

	private:
		TemperatureSensor();

#if defined HW_VERSION_2_2
		static NL::LM75BD *lm75;
#endif

		static bool initialized;
	};
}

#endif