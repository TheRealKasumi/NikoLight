/**
 * @file DS18B20.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::DS18B20}.
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
#include "hardware/DS18B20.h"

/**
 * @brief Create a new instance of {@link TL::DS18B20}.
 * @param busPin physical pin for the data bus
 */
TL::DS18B20::DS18B20(const uint8_t busPin)
{
	this->oneWire.reset(new OneWire(busPin));
}

/**
 * @brief Delete the {@link TL::DS18B20} instance.
 */
TL::DS18B20::~DS18B20()
{
}

/**
 * @brief Initialize the {@link TL::DS18B20}.
 * @return true when successful
 * @return false when there was an error or no sensor was found
 */
bool TL::DS18B20::begin()
{
	std::vector<uint64_t> sensorAddress;
	sensorAddress.reserve(10);
	if (!this->getSensors(sensorAddress))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to search sensors. The OneWire bus could be faulty."));
		return false;
	}

	if (sensorAddress.size() == 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Search completed but no sensors found."));
		return true;
	}

	this->sensorAddress.resize(sensorAddress.size());
	this->resolution.resize(sensorAddress.size());
	this->lastMeasurement.resize(sensorAddress.size());
	this->measurementReadyTime.resize(sensorAddress.size());
	for (size_t i = 0; i < sensorAddress.size(); i++)
	{
		this->sensorAddress.at(i) = sensorAddress.at(i);
		this->resolution.at(i) = TL::DS18B20::DS18B20Res::DS18B20_12_BIT;
		this->lastMeasurement.at(i) = 0.0f;
		this->measurementReadyTime.at(i) = ULONG_MAX;
	}

	return true;
}

/**
 * @brief Get the number of available sensors.
 * @return number of sensors
 */
size_t TL::DS18B20::getNumSensors()
{
	return this->sensorAddress.size();
}

/**
 * @brief Get a sensor address by it's index.
 * @param sensorIndex index of the sensor
 * @return address of the sensor
 */
uint64_t TL::DS18B20::getSensorAddress(const size_t sensorIndex)
{
	if (sensorIndex >= this->sensorAddress.size())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Sensor index out of bounds."));
		return 0;
	}

	return this->sensorAddress.at(sensorIndex);
}

/**
 * @brief Set the resolution of a specific sensor by it's index.
 * @param sensorIndex index of the sensor
 * @param resolution resolution to set
 * @return true when successful
 * @return false when there was an error
 */
bool TL::DS18B20::setResolution(const size_t sensorIndex, const TL::DS18B20::DS18B20Res resolution)
{
	if (sensorIndex >= this->sensorAddress.size())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Sensor index out of bounds."));
		return false;
	}

	if (!this->oneWire->reset())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to reset OneWire bus. The bus lines might be shorted or a sensor is malfunctioning."));
		return false;
	}

	this->oneWire->select((uint8_t *)&this->sensorAddress.at(sensorIndex));
	this->oneWire->write(0x4E, 0);
	this->oneWire->write(0x00, 0);
	this->oneWire->write(0x00, 0);
	this->oneWire->write(resolution);

	this->resolution.at(sensorIndex) = resolution;
	return true;
}

/**
 * @brief Get the resolution of a specific sensor by it's index.
 * @param sensorIndex index of the sensor
 * @param resolution variable to hold the resolution
 * @return true when successful
 * @return false when there was an error
 */
bool TL::DS18B20::getResolution(const size_t sensorIndex, TL::DS18B20::DS18B20Res &resolution)
{
	if (sensorIndex >= this->sensorAddress.size())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Sensor index out of bounds."));
		return false;
	}
	return this->resolution.at(sensorIndex);
}

/**
 * @brief Start a temperature measurement for a sensor with a given resolution.
 * @param sensorIndex index of the sensor
 * @param resolution resolution for the measurement
 * @return true when successful
 * @return false when there was an error
 */
bool TL::DS18B20::startMeasurement(const size_t sensorIndex)
{
	if (sensorIndex >= this->sensorAddress.size())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Sensor index out of bounds."));
		return false;
	}

	if (!this->oneWire->reset())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to reset OneWire bus. The bus lines might be shorted or a sensor is malfunctioning."));
		return false;
	}

	this->oneWire->select((uint8_t *)&this->sensorAddress.at(sensorIndex));
	this->oneWire->write(0x44, 0);

	if (this->resolution.at(sensorIndex) == TL::DS18B20::DS18B20Res::DS18B20_9_BIT)
	{
		this->measurementReadyTime.at(sensorIndex) = millis() + 110;
	}
	else if (this->resolution.at(sensorIndex) == TL::DS18B20::DS18B20Res::DS18B20_10_BIT)
	{
		this->measurementReadyTime.at(sensorIndex) = millis() + 200;
	}
	else if (this->resolution.at(sensorIndex) == TL::DS18B20::DS18B20Res::DS18B20_11_BIT)
	{
		this->measurementReadyTime.at(sensorIndex) = millis() + 400;
	}
	else if (this->resolution.at(sensorIndex) == TL::DS18B20::DS18B20Res::DS18B20_12_BIT)
	{
		this->measurementReadyTime.at(sensorIndex) = millis() + 800;
	}
	return true;
}

/**
 * @brief Check if the measurement for a specific sensor is ready.
 * @param sensorIndex index of the sensor
 * @return true when the sensor is ready to be read
 * @return false when it is not ready
 */
bool TL::DS18B20::isMeasurementReady(const size_t sensorIndex)
{
	if (sensorIndex >= this->sensorAddress.size())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Sensor index out of bounds."));
		return false;
	}
	return millis() >= this->measurementReadyTime.at(sensorIndex);
}

/**
 * @brief Read the temperature value from a specific sensor.
 * @param sensorIndex index of the sensor to read
 * @param temp variable to hold the temperature
 * @return true when successful
 * @return false when there was an error
 */
bool TL::DS18B20::getTemperature(const size_t sensorIndex, float &temp)
{
	if (sensorIndex >= this->sensorAddress.size())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Sensor index out of bounds."));
		temp = 0.0f;
		return false;
	}

	if (!this->isMeasurementReady(sensorIndex))
	{
		TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Sensor is not ready. Return previous measurement."));
		temp = this->lastMeasurement.at(sensorIndex);
		return true;
	}

	if (!this->oneWire->reset())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to reset OneWire bus. The bus lines might be shorted or a sensor is malfunctioning."));
		return false;
	}

	this->oneWire->select((uint8_t *)&this->sensorAddress.at(sensorIndex));
	this->oneWire->write(0xBE);

	uint8_t data[9] = {0};
	for (uint8_t i = 0; i < 9; i++)
	{
		data[i] = this->oneWire->read();
	}

	if (OneWire::crc8(data, 8) != data[8])
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The CRC of the sensors address is invalid. Check the OneWire bus."));
		temp = 0.0f;
		return false;
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

	temp = (float)raw / 16.0f;
	this->lastMeasurement.at(sensorIndex) = temp;
	return true;
}

/**
 * @brief Count all DS18B20 sensors on the bus and get their addresses.
 * @param sensorAddresses reference to the vector holding the sensor addresses
 * @return true when successful
 * @return false when there was an error
 */
bool TL::DS18B20::getSensors(std::vector<uint64_t> &sensorAddress)
{
	sensorAddress.clear();
	uint64_t address;
	this->oneWire->reset_search();
	while (this->oneWire->search((uint8_t *)&address))
	{
		if (OneWire::crc8((uint8_t *)&address, 7) != ((uint8_t *)&address)[7])
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The CRC of the sensors address is invalid. Check the OneWire bus."));
			return false;
		}

		if (((uint8_t *)&address)[0] != 0x28)
		{
			continue;
		}

		sensorAddress.push_back(address);
	}
	return true;
}