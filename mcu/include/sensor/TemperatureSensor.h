/**
 * @file TemperatureSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the temperature of the voltage regulator.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <stdint.h>
#include "configuration/SystemConfiguration.h"
#include "hardware/DS18B20.h"
#include "logging/Logger.h"

namespace TesLight
{
	class TemperatureSensor
	{
	public:
		TemperatureSensor();
		~TemperatureSensor();

		uint8_t getNumSensors();
		bool getMinTemperature(float &temp);
		bool getMaxTemperature(float &temp);
		bool getAverageTemperature(float &temp);

	private:
		TesLight::DS18B20 *ds18b20;
	};
}

#endif