/**
 * @file TemperatureSensor.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::TemperatureSensor}.
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
#include "sensor/TemperatureSensor.h"

bool NL::TemperatureSensor::initialized = false;

#ifdef HW_VERSION_2_2
NL::LM75BD *NL::TemperatureSensor::lm75 = nullptr;
#endif

/**
 * @brief Start the temperature sensor.
 * @return OK when the temperature sensor was initialized
 * @return ERROR_DS18B20_UNAVAILABLE when no temperature sensor is available
 */
#if defined(HW_VERSION_1_0) || defined(HW_VERSION_2_0) || defined(HW_VERSION_2_1)
NL::TemperatureSensor::Error NL::TemperatureSensor::begin()
{
	NL::TemperatureSensor::initialized = false;
	if (!NL::DS18B20::isInitialized() || NL::DS18B20::getNumSensors() == 0)
	{
		return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
	}

	for (size_t i = 0; i < NL::DS18B20::getNumSensors(); i++)
	{
		if (NL::DS18B20::setResolution(NL::DS18B20::DS18B20Res::DS18B20_10_BIT, i) != NL::DS18B20::Error::OK)
		{
			return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}
		if (NL::DS18B20::startMeasurement(i) != NL::DS18B20::Error::OK)
		{
			return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}
	}

	NL::TemperatureSensor::initialized = true;
	return NL::TemperatureSensor::Error::OK;
}
#elif defined(HW_VERSION_2_2)
NL::TemperatureSensor::Error NL::TemperatureSensor::begin(NL::LM75BD *lm75)
{
	NL::TemperatureSensor::initialized = false;
	NL::TemperatureSensor::lm75 = lm75;
	if (lm75 == nullptr)
	{
		return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
	}

	NL::TemperatureSensor::initialized = true;
	return NL::TemperatureSensor::Error::OK;
}
#endif

/**
 * @brief Stop the temperature sensor.
 */
void NL::TemperatureSensor::end()
{
	NL::TemperatureSensor::initialized = false;
}

/**
 * @brief Check if the temperature sensor was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::TemperatureSensor::isInitialized()
{
	return NL::TemperatureSensor::initialized;
}

/**
 * @brief Get the minimum temperature from all sensors.
 * @param temp variable to hold the temperature value, will be 0.0 if no sensor is present
 * @return OK when the temperature sensor was initialized
 * @return ERROR_DS18B20_UNAVAILABLE when no temperature sensor is available
 */
NL::TemperatureSensor::Error NL::TemperatureSensor::getMinTemperature(float &temp)
{
	if (!NL::DS18B20::isInitialized() || NL::DS18B20::getNumSensors() == 0)
	{
		temp = 0.0f;
		return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
	}

	temp = 1000.0f;
	for (size_t i = 0; i < NL::DS18B20::getNumSensors(); i++)
	{
		float currentTemp;
		if (NL::DS18B20::getTemperature(currentTemp, i) != NL::DS18B20::Error::OK)
		{
			return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}

		bool isReady;
		if (NL::DS18B20::isMeasurementReady(isReady, i) != NL::DS18B20::Error::OK)
		{
			return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}
		if (isReady)
		{
			if (NL::DS18B20::startMeasurement(i) != NL::DS18B20::Error::OK)
			{
				return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
			}
		}

		if (currentTemp < temp)
		{
			temp = currentTemp;
		}
	}

	return NL::TemperatureSensor::Error::OK;
}

/**
 * @brief Get the maximum temperature of all sensors.
 * @param temp variable to hold the temperature value, will be 0.0 if no sensor is present
 * @return OK when the temperature sensor was initialized
 * @return ERROR_DS18B20_UNAVAILABLE when no temperature sensor is available
 */
NL::TemperatureSensor::Error NL::TemperatureSensor::getMaxTemperature(float &temp)
{
#if defined(HW_VERSION_1_0) || defined(HW_VERSION_2_0) || defined(HW_VERSION_2_1)
	if (!NL::DS18B20::isInitialized() || NL::DS18B20::getNumSensors() == 0)
	{
		temp = 0.0f;
		return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
	}

	temp = -1000.0f;
	for (size_t i = 0; i < NL::DS18B20::getNumSensors(); i++)
	{
		float currentTemp = 0.0f;
		if (NL::DS18B20::getTemperature(currentTemp, i) != NL::DS18B20::Error::OK)
		{
			return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}

		bool isReady;
		if (NL::DS18B20::isMeasurementReady(isReady, i) != NL::DS18B20::Error::OK)
		{
			return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}
		if (isReady)
		{
			if (NL::DS18B20::startMeasurement(i) != NL::DS18B20::Error::OK)
			{
				return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
			}
		}

		if (currentTemp > temp)
		{
			temp = currentTemp;
		}
	}
#elif defined(HW_VERSION_2_2)
	if (NL::TemperatureSensor::lm75 == nullptr)
	{
		temp = 0.0f;
		return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
	}

	temp = NL::TemperatureSensor::lm75->getTemperature();
#endif

	return NL::TemperatureSensor::Error::OK;
}

/**
 * @brief Get the average temperature from all sensors.
 * @param temp variable to hold the temperature value, will be 0.0 if no sensor is present
 * @return OK when the temperature sensor was initialized
 * @return ERROR_DS18B20_UNAVAILABLE when no temperature sensor is available
 */
NL::TemperatureSensor::Error NL::TemperatureSensor::getAverageTemperature(float &temp)
{
	if (!NL::DS18B20::isInitialized() || NL::DS18B20::getNumSensors() == 0)
	{
		temp = 0.0f;
		return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
	}

	temp = 0.0f;
	for (size_t i = 0; i < NL::DS18B20::getNumSensors(); i++)
	{
		float currentTemp = 0.0f;
		if (NL::DS18B20::getTemperature(currentTemp, i) != NL::DS18B20::Error::OK)
		{
			return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}

		bool isReady;
		if (NL::DS18B20::isMeasurementReady(isReady, i) != NL::DS18B20::Error::OK)
		{
			return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
		}
		if (isReady)
		{
			if (NL::DS18B20::startMeasurement(i) != NL::DS18B20::Error::OK)
			{
				return NL::TemperatureSensor::Error::ERROR_DS18B20_UNAVAILABLE;
			}
		}

		temp += currentTemp;
	}
	temp / NL::DS18B20::getNumSensors();

	return NL::TemperatureSensor::Error::OK;
}
