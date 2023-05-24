/**
 * @file DS18B20.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::DS18B20}.
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
#include "hardware/DS18B20.h"

bool NL::DS18B20::initialized = false;
OneWire NL::DS18B20::oneWire;
std::vector<uint64_t> NL::DS18B20::sensorAddress;
std::vector<NL::DS18B20::DS18B20Res> NL::DS18B20::resolution;
std::vector<float> NL::DS18B20::lastMeasurement;
std::vector<unsigned long> NL::DS18B20::measurementReadyTime;

/**
 * @brief Start the DS18B20 sensors.
 * @return OK when the sensors were initialized
 * @return ERROR_OW_COMM when the OneWire communication failed
 * @return ERROR_OW_CRC when the crc check failed
 */
NL::DS18B20::Error NL::DS18B20::begin(const uint8_t busPin)
{
	NL::DS18B20::initialized = false;
	NL::DS18B20::oneWire = OneWire(busPin);
	std::vector<uint64_t> sensorAddress;
	sensorAddress.reserve(10);

	const NL::DS18B20::Error getSensorError = NL::DS18B20::getSensors(sensorAddress);
	if (getSensorError != NL::DS18B20::Error::OK)
	{
		return getSensorError;
	}

	NL::DS18B20::sensorAddress = sensorAddress;
	NL::DS18B20::resolution.assign(sensorAddress.size(), NL::DS18B20::DS18B20Res::DS18B20_12_BIT);
	NL::DS18B20::lastMeasurement.assign(sensorAddress.size(), 0.0f);
	NL::DS18B20::measurementReadyTime.assign(sensorAddress.size(), ULONG_MAX);

	NL::DS18B20::initialized = true;
	return NL::DS18B20::Error::OK;
}

/**
 * @brief Stop the DS18B20 sensors and free memory.
 */
void NL::DS18B20::end()
{
	NL::DS18B20::initialized = false;
	NL::DS18B20::sensorAddress.clear();
	NL::DS18B20::resolution.clear();
	NL::DS18B20::lastMeasurement.clear();
	NL::DS18B20::measurementReadyTime.clear();
}

/**
 * @brief Return if the sensor was initialized
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::DS18B20::isInitialized()
{
	return NL::DS18B20::initialized;
}

/**
 * @brief Get the number of available sensors.
 * @return number of sensors
 */
size_t NL::DS18B20::getNumSensors()
{
	return NL::DS18B20::sensorAddress.size();
}

/**
 * @brief Get a sensor address by it's index or 0 when it's out of bounds.
 * @param sensorAdress reference to variable which the address will be written to
 * @param sensorIndex index of the sensor
 * @return OK when the address was read
 * @return ERROR_OUT_OF_BOUNDS when the index is out of bounds
 */
NL::DS18B20::Error NL::DS18B20::getSensorAddress(uint64_t &sensorAddress, const size_t sensorIndex)
{
	if (sensorIndex >= NL::DS18B20::sensorAddress.size())
	{
		sensorAddress = 0;
		return NL::DS18B20::Error::ERROR_OUT_OF_BOUNDS;
	}

	sensorAddress = NL::DS18B20::sensorAddress.at(sensorIndex);
	return NL::DS18B20::Error::OK;
}

/**
 * @brief Set the resolution of a specific sensor by it's index.
 * @param resolution resolution to set
 * @param sensorIndex index of the sensor
 * @return OK when the resolution was set
 * @return ERROR_OW_COMM when the OneWire communication failed
 */
NL::DS18B20::Error NL::DS18B20::setResolution(const NL::DS18B20::DS18B20Res resolution, const size_t sensorIndex)
{
	if (sensorIndex >= NL::DS18B20::sensorAddress.size())
	{
		return NL::DS18B20::Error::ERROR_OUT_OF_BOUNDS;
	}

	if (!NL::DS18B20::oneWire.reset())
	{
		return NL::DS18B20::Error::ERROR_OW_COMM;
	}

	NL::DS18B20::oneWire.select((uint8_t *)&NL::DS18B20::sensorAddress.at(sensorIndex));
	NL::DS18B20::oneWire.write(0x4E, 0);
	NL::DS18B20::oneWire.write(0x00, 0);
	NL::DS18B20::oneWire.write(0x00, 0);
	NL::DS18B20::oneWire.write(static_cast<uint8_t>(resolution));
	NL::DS18B20::resolution.at(sensorIndex) = resolution;

	return NL::DS18B20::Error::OK;
}

/**
 * @brief Get the resolution of a specific sensor by it's index.
 * @param resolution reference variable to hold the resolution
 * @param sensorIndex index of the sensor
 * @return OK when the resolution was set
 * @return ERROR_OUT_OF_BOUNDS when the index is out of bounds
 */
NL::DS18B20::Error NL::DS18B20::getResolution(NL::DS18B20::DS18B20Res &resolution, const size_t sensorIndex)
{
	if (sensorIndex >= NL::DS18B20::sensorAddress.size())
	{
		resolution = NL::DS18B20::DS18B20Res::DS18B20_9_BIT;
		return NL::DS18B20::Error::ERROR_OUT_OF_BOUNDS;
	}
	resolution = NL::DS18B20::resolution.at(sensorIndex);

	return NL::DS18B20::Error::OK;
}

/**
 * @brief Start a temperature measurement for a sensor with a given resolution.
 * @param sensorIndex index of the sensor
 * @param resolution resolution for the measurement
 * @return OK when the measurement was started
 * @return ERROR_OUT_OF_BOUNDS when the sensor index is out of bounds
 * @return ERROR_OW_COMM when the OneWire communication failed
 */
NL::DS18B20::Error NL::DS18B20::startMeasurement(const size_t sensorIndex)
{
	if (sensorIndex >= NL::DS18B20::sensorAddress.size())
	{
		return NL::DS18B20::Error::ERROR_OUT_OF_BOUNDS;
	}

	if (!NL::DS18B20::oneWire.reset())
	{
		return NL::DS18B20::Error::ERROR_OW_COMM;
	}

	NL::DS18B20::oneWire.select((uint8_t *)&NL::DS18B20::sensorAddress.at(sensorIndex));
	NL::DS18B20::oneWire.write(0x44, 0);
	if (NL::DS18B20::resolution.at(sensorIndex) == NL::DS18B20::DS18B20Res::DS18B20_9_BIT)
	{
		NL::DS18B20::measurementReadyTime.at(sensorIndex) = millis() + 110;
	}
	else if (NL::DS18B20::resolution.at(sensorIndex) == NL::DS18B20::DS18B20Res::DS18B20_10_BIT)
	{
		NL::DS18B20::measurementReadyTime.at(sensorIndex) = millis() + 200;
	}
	else if (NL::DS18B20::resolution.at(sensorIndex) == NL::DS18B20::DS18B20Res::DS18B20_11_BIT)
	{
		NL::DS18B20::measurementReadyTime.at(sensorIndex) = millis() + 400;
	}
	else if (NL::DS18B20::resolution.at(sensorIndex) == NL::DS18B20::DS18B20Res::DS18B20_12_BIT)
	{
		NL::DS18B20::measurementReadyTime.at(sensorIndex) = millis() + 800;
	}

	return NL::DS18B20::Error::OK;
}

/**
 * @brief Check if the measurement for a specific sensor is ready.
 * @param isReady reference variable holding the ready status of the sensor
 * @param sensorIndex index of the sensor
 * @return OK when the function call was successful
 * @return ERROR_OUT_OF_BOUNDS when the sensor index is out of bounds
 */
NL::DS18B20::Error NL::DS18B20::isMeasurementReady(bool &isReady, const size_t sensorIndex)
{
	if (sensorIndex >= NL::DS18B20::sensorAddress.size())
	{
		isReady = false;
		return NL::DS18B20::Error::ERROR_OUT_OF_BOUNDS;
	}
	isReady = millis() >= NL::DS18B20::measurementReadyTime.at(sensorIndex);
	return NL::DS18B20::Error::OK;
}

/**
 * @brief Read the temperature value from a specific sensor.
 * @param sensorIndex index of the sensor to read
 * @param temp variable to hold the temperature
 * @return OK when the temperature was read
 * @return ERROR_OUT_OF_BOUNDS when the sensor index is out of bounds
 * @return ERROR_OW_COMM when the OneWire communication failed
 * @return ERROR_OW_CRC when the crc check failed
 */
NL::DS18B20::Error NL::DS18B20::getTemperature(float &temp, const size_t sensorIndex)
{
	if (sensorIndex >= NL::DS18B20::sensorAddress.size())
	{
		temp = 0.0f;
		return NL::DS18B20::Error::ERROR_OUT_OF_BOUNDS;
	}

	bool measurementReady;
	const NL::DS18B20::Error readyCheckError = NL::DS18B20::isMeasurementReady(measurementReady, sensorIndex);
	if (readyCheckError != NL::DS18B20::Error::OK)
	{
		return readyCheckError;
	}
	if (!measurementReady)
	{
		temp = NL::DS18B20::lastMeasurement.at(sensorIndex);
		return NL::DS18B20::Error::OK;
	}

	if (!NL::DS18B20::oneWire.reset())
	{
		return NL::DS18B20::Error::ERROR_OW_COMM;
	}

	NL::DS18B20::oneWire.select((uint8_t *)&NL::DS18B20::sensorAddress.at(sensorIndex));
	NL::DS18B20::oneWire.write(0xBE);

	uint8_t data[9] = {0};
	for (uint8_t i = 0; i < 9; i++)
	{
		data[i] = NL::DS18B20::oneWire.read();
	}

	if (OneWire::crc8(data, 8) != data[8])
	{
		temp = 0.0f;
		return NL::DS18B20::Error::ERROR_OW_CRC;
	}

	int16_t raw = (data[1] << 8) | data[0];
	const uint8_t resolution = (data[4] & 0x60);
	if (resolution == 0x00) // 9 bit
	{
		raw = raw & ~7;
	}
	else if (resolution == 0x20) // 10 bit
	{
		raw = raw & ~3;
	}
	else if (resolution == 0x40) // 11 bit
	{
		raw = raw & ~1;
	}

	temp = raw / 16.0f;
	NL::DS18B20::lastMeasurement.at(sensorIndex) = temp;
	return NL::DS18B20::Error::OK;
}

/**
 * @brief Count all DS18B20 sensors on the bus and get their addresses.
 * @param sensorAddresses reference to the vector holding the sensor addresses
 * @return OK when the sensor list was received
 * @return ERROR_OW_CRC when the crc check failed
 */
NL::DS18B20::Error NL::DS18B20::getSensors(std::vector<uint64_t> &sensorAddress)
{
	sensorAddress.clear();
	uint64_t address;
	NL::DS18B20::oneWire.reset_search();
	while (NL::DS18B20::oneWire.search((uint8_t *)&address))
	{
		if (OneWire::crc8((uint8_t *)&address, 7) != ((uint8_t *)&address)[7])
		{
			return NL::DS18B20::Error::ERROR_OW_CRC;
		}

		if (((uint8_t *)&address)[0] != 0x28)
		{
			continue;
		}

		sensorAddress.push_back(address);
	}
	return NL::DS18B20::Error::OK;
}
