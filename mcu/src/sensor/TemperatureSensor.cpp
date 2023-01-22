/**
 * @file TemperatureSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::TemperatureSensor}.
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
#include "sensor/TemperatureSensor.h"

bool TL::TemperatureSensor::initialized = false;

/**
 * @brief Start the temperature sensor.
 * @return OK when the temperature sensor was initialized
 * @return ERROR_DS18B20_UNAVAILABLE when no temperature sensor is available
 */
TL::TemperatureSensor::Error TL::TemperatureSensor::begin()
{
	TL::TemperatureSensor::initialized = false;
	if (!TL::DS18B20::isInitialized() || TL::DS18B20::getNumSensors() == 0)
	{
		return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
	}

	for (size_t i = 0; i < TL::DS18B20::getNumSensors(); i++)
	{
		if (TL::DS18B20::setResolution(TL::DS18B20::DS18B20Res::DS18B20_10_BIT, i) != TL::DS18B20::Error::OK)
		{
			return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}
		if (TL::DS18B20::startMeasurement(i) != TL::DS18B20::Error::OK)
		{
			return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}
	}

	TL::TemperatureSensor::initialized = true;
	return TL::TemperatureSensor::Error::OK;
}

/**
 * @brief Stop the temperature sensor.
 */
void TL::TemperatureSensor::end()
{
	TL::TemperatureSensor::initialized = false;
}

/**
 * @brief Check if the temperature sensor was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::TemperatureSensor::isInitialized()
{
	return TL::TemperatureSensor::initialized;
}

/**
 * @brief Get the minimum temperature from all sensors.
 * @param temp variable to hold the temperature value, will be 0.0 if no sensor is present
 * @return OK when the temperature sensor was initialized
 * @return ERROR_DS18B20_UNAVAILABLE when no temperature sensor is available
 */
TL::TemperatureSensor::Error TL::TemperatureSensor::getMinTemperature(float &temp)
{
	if (!TL::DS18B20::isInitialized() || TL::DS18B20::getNumSensors() == 0)
	{
		temp = 0.0f;
		return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
	}

	temp = 1000.0f;
	for (size_t i = 0; i < TL::DS18B20::getNumSensors(); i++)
	{
		float currentTemp;
		if (TL::DS18B20::getTemperature(currentTemp, i) != TL::DS18B20::Error::OK)
		{
			return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}

		bool isReady;
		if (TL::DS18B20::isMeasurementReady(isReady, i) != TL::DS18B20::Error::OK)
		{
			return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}
		if (isReady)
		{
			if (TL::DS18B20::startMeasurement(i) != TL::DS18B20::Error::OK)
			{
				return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
			}
		}

		if (currentTemp < temp)
		{
			temp = currentTemp;
		}
	}

	return TL::TemperatureSensor::Error::OK;
}

/**
 * @brief Get the maximum temperature of all sensors.
 * @param temp variable to hold the temperature value, will be 0.0 if no sensor is present
 * @return OK when the temperature sensor was initialized
 * @return ERROR_DS18B20_UNAVAILABLE when no temperature sensor is available
 */
TL::TemperatureSensor::Error TL::TemperatureSensor::getMaxTemperature(float &temp)
{
	if (!TL::DS18B20::isInitialized() || TL::DS18B20::getNumSensors() == 0)
	{
		temp = 0.0f;
		return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
	}

	temp = -1000.0f;
	for (size_t i = 0; i < TL::DS18B20::getNumSensors(); i++)
	{
		float currentTemp = 0.0f;
		if (TL::DS18B20::getTemperature(currentTemp, i) != TL::DS18B20::Error::OK)
		{
			return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}

		bool isReady;
		if (TL::DS18B20::isMeasurementReady(isReady, i) != TL::DS18B20::Error::OK)
		{
			return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}
		if (isReady)
		{
			if (TL::DS18B20::startMeasurement(i) != TL::DS18B20::Error::OK)
			{
				return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
			}
		}

		if (currentTemp > temp)
		{
			temp = currentTemp;
		}
	}

	return TL::TemperatureSensor::Error::OK;
}

/**
 * @brief Get the average temperature from all sensors.
 * @param temp variable to hold the temperature value, will be 0.0 if no sensor is present
 * @return OK when the temperature sensor was initialized
 * @return ERROR_DS18B20_UNAVAILABLE when no temperature sensor is available
 */
TL::TemperatureSensor::Error TL::TemperatureSensor::getAverageTemperature(float &temp)
{
	if (!TL::DS18B20::isInitialized() || TL::DS18B20::getNumSensors() == 0)
	{
		temp = 0.0f;
		return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
	}

	temp = 0.0f;
	for (size_t i = 0; i < TL::DS18B20::getNumSensors(); i++)
	{
		float currentTemp = 0.0f;
		if (TL::DS18B20::getTemperature(currentTemp, i) != TL::DS18B20::Error::OK)
		{
			return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}

		bool isReady;
		if (TL::DS18B20::isMeasurementReady(isReady, i) != TL::DS18B20::Error::OK)
		{
			return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}
		if (isReady)
		{
			if (TL::DS18B20::startMeasurement(i) != TL::DS18B20::Error::OK)
			{
				return TL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
			}
		}

		temp += currentTemp;
	}
	temp / TL::DS18B20::getNumSensors();

	return TL::TemperatureSensor::Error::OK;
}
