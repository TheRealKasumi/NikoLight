/**
 * @file TemperatureSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the temperature from the DS18B20 sensors.
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
#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <stdint.h>

#include "configuration/SystemConfiguration.h"
#include "hardware/DS18B20.h"

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

		static NL::TemperatureSensor::Error begin();
		static void end();
		static bool isInitialized();

		static NL::TemperatureSensor::Error getMinTemperature(float &temp);
		static NL::TemperatureSensor::Error getMaxTemperature(float &temp);
		static NL::TemperatureSensor::Error getAverageTemperature(float &temp);

	private:
		TemperatureSensor();

		static bool initialized;
	};
}

#endif