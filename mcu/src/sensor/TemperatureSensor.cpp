/**
 * @file TemperatureSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::TemperatureSensor}.
 *
 * @copyright Copyright (c) 2022
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
