/**
 * @file TemperatureSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::TemperatureSensor}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "sensor/TemperatureSensor.h"

/**
 * @brief Create a new instance of {@link TesLight::TemperatureSensor}.
 */
TesLight::TemperatureSensor::TemperatureSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize temperature sensor."));
	this->ds18b20 = new TesLight::DS18B20(ONE_WIRE_PIN);
	if (!this->ds18b20->begin())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize temperature sensor."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Temperature sensor initialized, set sensor resolution and start measurement."));
	for (uint8_t i = 0; i < this->ds18b20->getNumSensors(); i++)
	{
		if (!this->ds18b20->setResolution(i, TEMP_SENSOR_RESOLUTION))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to set temperature sensor resolution."));
		}
		if (!this->ds18b20->startMeasurement(i))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start the temperature measurement."));
		}
	}
}

/**
 * @brief Delete the {@link TesLight::TemperatureSensor} instance and free resources.
 */
TesLight::TemperatureSensor::~TemperatureSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("delete temperature sensor and free resources."));
	if (this->ds18b20)
	{
		delete this->ds18b20;
		this->ds18b20 = nullptr;
	}
}

/**
 * @brief Get the number of temperature sensors.
 * @return number of sensors
 */
uint8_t TesLight::TemperatureSensor::getNumSensors()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get number of detected sensors."));
	return this->ds18b20->getNumSensors();
}

/**
 * @brief Get the minimum temperature from all sensors.
 * @param temp variable to hold the temperature value
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::TemperatureSensor::getMinTemperature(float &temp)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get the minimum temperature."));

	if (this->ds18b20->getNumSensors() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("No temperature sensors present."));
		temp = 0.0f;
		return true;
	}

	temp = 1000.0f;
	for (uint8_t i = 0; i < this->ds18b20->getNumSensors(); i++)
	{
		float currentTemp;
		if (!this->ds18b20->getTemperature(i, currentTemp))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to get temperature from sensor ") + i + F("."));
			return false;
		}

		if (this->ds18b20->isMeasurementReady(i))
		{
			if (!this->ds18b20->startMeasurement(i))
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to start temperature measurement on sensor ") + i + F("."));
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
 * @param temp variable to hold the temperature value
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::TemperatureSensor::getMaxTemperature(float &temp)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get the maximum temperature."));

	if (this->ds18b20->getNumSensors() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("No temperature sensors present."));
		temp = 0.0f;
		return true;
	}

	temp = -1000.0f;
	for (uint8_t i = 0; i < this->ds18b20->getNumSensors(); i++)
	{
		float currentTemp = 0.0f;
		if (!this->ds18b20->getTemperature(i, currentTemp))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to get temperature from sensor ") + i + F("."));
			return false;
		}

		if (this->ds18b20->isMeasurementReady(i))
		{
			if (!this->ds18b20->startMeasurement(i))
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to start temperature measurement on sensor ") + i + F("."));
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
 * @param temp variable to hold the temperature value
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::TemperatureSensor::getAverageTemperature(float &temp)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get the average temperature."));

	if (this->ds18b20->getNumSensors() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("No temperature sensors present."));
		temp = 0.0f;
		return true;
	}

	temp = 0.0f;
	for (uint8_t i = 0; i < this->ds18b20->getNumSensors(); i++)
	{
		float currentTemp = 0.0f;
		if (!this->ds18b20->getTemperature(i, currentTemp))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to get temperature from sensor ") + i + F("."));
			return false;
		}

		if (this->ds18b20->isMeasurementReady(i))
		{
			if (!this->ds18b20->startMeasurement(i))
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, (String)F("Failed to start temperature measurement on sensor ") + i + F("."));
				return false;
			}
		}

		temp += currentTemp;
	}
	temp / this->ds18b20->getNumSensors();
	return true;
}
