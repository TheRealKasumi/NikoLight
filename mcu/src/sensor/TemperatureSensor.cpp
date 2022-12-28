/**
 * @file TemperatureSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::TemperatureSensor}.
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
#include "sensor/TemperatureSensor.h"

/**
 * @brief Create a new instance of {@link TL::TemperatureSensor}.
 */
TL::TemperatureSensor::TemperatureSensor()
{
	this->ds18b20.reset(new TL::DS18B20(ONE_WIRE_PIN));
	if (!this->ds18b20->begin())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize temperature sensor."));
		return;
	}

	for (uint8_t i = 0; i < this->ds18b20->getNumSensors(); i++)
	{
		if (!this->ds18b20->setResolution(i, (TL::DS18B20::DS18B20Res)TEMP_SENSOR_RESOLUTION))
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to set temperature sensor resolution."));
		}
		if (!this->ds18b20->startMeasurement(i))
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start the temperature measurement."));
		}
	}
}

/**
 * @brief Delete the {@link TL::TemperatureSensor} instance.
 */
TL::TemperatureSensor::~TemperatureSensor()
{
}

/**
 * @brief Get the number of temperature sensors.
 * @return number of sensors
 */
uint8_t TL::TemperatureSensor::getNumSensors()
{
	return this->ds18b20->getNumSensors();
}

/**
 * @brief Get the minimum temperature from all sensors.
 * @param temp variable to hold the temperature value, will be 0.0 if no sensor is present
 * @return true when successful
 * @return false when there was an error
 */
bool TL::TemperatureSensor::getMinTemperature(float &temp)
{
	if (this->ds18b20->getNumSensors() == 0)
	{
		temp = 0.0f;
		return true;
	}

	temp = 1000.0f;
	for (uint8_t i = 0; i < this->ds18b20->getNumSensors(); i++)
	{
		float currentTemp;
		if (!this->ds18b20->getTemperature(i, currentTemp))
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to get temperature from sensor ") + i + F("."));
			return false;
		}

		if (this->ds18b20->isMeasurementReady(i))
		{
			if (!this->ds18b20->startMeasurement(i))
			{
				TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to start temperature measurement on sensor ") + i + F("."));
				return false;
			}
		}

		if (currentTemp < temp)
		{
			temp = currentTemp;
		}
	}
	return true;
}

/**
 * @brief Get the maximum temperature of all sensors.
 * @param temp variable to hold the temperature value, will be 0.0 if no sensor is present
 * @return true when successful
 * @return false when there was an error
 */
bool TL::TemperatureSensor::getMaxTemperature(float &temp)
{
	if (this->ds18b20->getNumSensors() == 0)
	{
		temp = 0.0f;
		return true;
	}

	temp = -1000.0f;
	for (uint8_t i = 0; i < this->ds18b20->getNumSensors(); i++)
	{
		float currentTemp = 0.0f;
		if (!this->ds18b20->getTemperature(i, currentTemp))
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to get temperature from sensor ") + i + F("."));
			return false;
		}

		if (this->ds18b20->isMeasurementReady(i))
		{
			if (!this->ds18b20->startMeasurement(i))
			{
				TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to start temperature measurement on sensor ") + i + F("."));
				return false;
			}
		}

		if (currentTemp > temp)
		{
			temp = currentTemp;
		}
	}
	return true;
}

/**
 * @brief Get the average temperature from all sensors.
 * @param temp variable to hold the temperature value, will be 0.0 if no sensor is present
 * @return true when successful
 * @return false when there was an error
 */
bool TL::TemperatureSensor::getAverageTemperature(float &temp)
{
	if (this->ds18b20->getNumSensors() == 0)
	{
		temp = 0.0f;
		return true;
	}

	temp = 0.0f;
	for (uint8_t i = 0; i < this->ds18b20->getNumSensors(); i++)
	{
		float currentTemp = 0.0f;
		if (!this->ds18b20->getTemperature(i, currentTemp))
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to get temperature from sensor ") + i + F("."));
			return false;
		}

		if (this->ds18b20->isMeasurementReady(i))
		{
			if (!this->ds18b20->startMeasurement(i))
			{
				TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to start temperature measurement on sensor ") + i + F("."));
				return false;
			}
		}

		temp += currentTemp;
	}
	temp / this->ds18b20->getNumSensors();
	return true;
}
