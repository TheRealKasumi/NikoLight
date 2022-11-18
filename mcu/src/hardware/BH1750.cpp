/**
 * @file BH1750.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::BH1750}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "hardware/BH1750.h"

/**
 * @brief Create a new instance of {@link TesLight::BH1750}.
 * @param deviceAddress I²C address of the device
 */
TesLight::BH1750::BH1750(const uint8_t deviceAddress)
{
	this->deviceAddress = deviceAddress;
	this->resolution = TesLight::BH1750::BH1750Res::BH1750_HIGH;
}

/**
 * @brief Create a new instance of {@link TesLight::BH1750}.
 * @param deviceAddress I²C address of the device
 * @param resolution resolution can be high or low
 */
TesLight::BH1750::BH1750(const uint8_t deviceAddress, const TesLight::BH1750::BH1750Res resolution)
{
	this->deviceAddress = deviceAddress;
	this->resolution = resolution;
}

/**
 * @brief Delete the {@link TesLight::BH1750} instance.
 */
TesLight::BH1750::~BH1750()
{
}

/**
 * @brief Initialize the sensor and start the measurement.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::BH1750::begin()
{
	if (!this->setResolution(this->resolution))
	{
		return false;
	}

	return true;
}

/**
 * @brief Set the sensor resolution.
 * @param resolution resolution can be high or low
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::BH1750::setResolution(const TesLight::BH1750::BH1750Res resolution)
{
	this->resolution = resolution;
	if (!this->write(this->resolution))
	{
		return false;
	}

	return true;
}

/**
 * @brief Get the currently set resolution.
 * @return resolution value
 */
TesLight::BH1750::BH1750Res TesLight::BH1750::getResolution()
{
	return this->resolution;
}

/**
 * @brief Get the current brightness in lux.
 * @param lux variable to store the brightness
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::BH1750::getLux(float &lux)
{
	uint16_t raw;
	if (!this->read(raw))
	{
		return 0.0f;
	}

	lux = (raw / 1.2f);

	return true;
}

/**
 * @brief Send a command to the BH1750 sensor.
 * @param command command code to send
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::BH1750::write(const uint8_t command)
{
	Wire.beginTransmission(this->deviceAddress);

	if (Wire.write(command) == 0)
	{
		return false;
	}

	if (Wire.endTransmission() != 0)
	{
		return false;
	}

	return true;
}

/**
 * @brief Read the brightness value from the BH1750 sensor.
 * @param value variable to store the brightness value
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::BH1750::read(uint16_t &value)
{
	uint8_t *bytes = (uint8_t *)&value;
	if (Wire.requestFrom(this->deviceAddress, 2) != 2)
	{
		return false;
	}

	if (Wire.available() != 2)
	{
		return false;
	}

	bytes[0] = Wire.read();
	bytes[1] = Wire.read();

	return true;
}
