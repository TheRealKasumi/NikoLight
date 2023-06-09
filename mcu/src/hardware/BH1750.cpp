/**
 * @file BH1750.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::BH1750}.
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
#include "hardware/BH1750.h"

bool NL::BH1750::initialized = false;
uint8_t NL::BH1750::deviceAddress;
NL::BH1750::BH1750Res NL::BH1750::resolution;

/**
 * @brief Initialize the light sensor and start the measurement.
 * @param deviceAddress I²C address of the device
 * @return OK when the sensor was found and initialized.
 * @return ERROR_IIC_COMM when the I²C communication failed
 */
NL::BH1750::Error NL::BH1750::begin(const uint8_t deviceAddress)
{
	return NL::BH1750::begin(deviceAddress, NL::BH1750::BH1750Res::BH1750_HIGH);
}

/**
 * @brief Initialize the light sensor and start the measurement.
 * @param deviceAddress I²C address of the device
 * @param resolution resolution can be high or low
 * @return OK when the sensor was found and initialized.
 * @return ERROR_IIC_COMM when the I²C communication failed
 */
NL::BH1750::Error NL::BH1750::begin(const uint8_t deviceAddress, const NL::BH1750::BH1750Res resolution)
{
	NL::BH1750::deviceAddress = deviceAddress;
	NL::BH1750::resolution = resolution;

	const NL::BH1750::Error error = BH1750::setResolution(NL::BH1750::resolution);
	NL::BH1750::initialized = error == NL::BH1750::Error::OK;
	return error;
}

/**
 * @brief Stop the light sensor.
 */
void NL::BH1750::end()
{
	NL::BH1750::initialized = false;
}

/**
 * @brief Return if the sensor was initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::BH1750::isInitialized()
{
	return NL::BH1750::initialized;
}

/**
 * @brief Set the sensor resolution.
 * @param resolution resolution can be high or low
 * @return OK when the resolution was set
 * @return ERROR_IIC_COMM when the I²C communication failed
 */
NL::BH1750::Error NL::BH1750::setResolution(const NL::BH1750::BH1750Res resolution)
{
	NL::BH1750::resolution = resolution;
	return NL::BH1750::write(static_cast<uint8_t>(NL::BH1750::resolution));
}

/**
 * @brief Get the currently set resolution.
 * @return resolution value
 */
NL::BH1750::BH1750Res NL::BH1750::getResolution()
{
	return NL::BH1750::resolution;
}

/**
 * @brief Get the current brightness in lux.
 * @param lux variable to store the brightness
 * @return OK when the resolution was set
 * @return ERROR_IIC_COMM when the I²C communication failed
 */
NL::BH1750::Error NL::BH1750::getLux(float &lux)
{
	uint16_t raw;
	const NL::BH1750::Error error = NL::BH1750::read(raw);
	if (error == NL::BH1750::Error::OK)
	{
		lux = (raw / 1.2f);
	}
	else
	{
		lux = 0.0f;
	}
	return error;
}

/**
 * @brief Send a command to the BH1750 sensor.
 * @param command command code to send
 * @return OK when the resolution was set
 * @return ERROR_IIC_COMM when the I²C communication failed
 */
NL::BH1750::Error NL::BH1750::write(const uint8_t command)
{
	Wire.beginTransmission(NL::BH1750::deviceAddress);
	if (Wire.write(command) == 0)
	{
		return NL::BH1750::Error::ERROR_IIC_COMM;
	}
	if (Wire.endTransmission() != 0)
	{
		return NL::BH1750::Error::ERROR_IIC_COMM;
	}
	return NL::BH1750::Error::OK;
}

/**
 * @brief Read the brightness value from the BH1750 sensor.
 * @param value variable to store the brightness value
 * @return OK when the resolution was set
 * @return ERROR_IIC_COMM when the I²C communication failed
 */
NL::BH1750::Error NL::BH1750::read(uint16_t &value)
{
	uint8_t *bytes = (uint8_t *)&value;
	if (Wire.requestFrom(static_cast<int>(NL::BH1750::deviceAddress), static_cast<int>(2)) != 2)
	{
		return NL::BH1750::Error::ERROR_IIC_COMM;
	}
	if (Wire.available() != 2)
	{
		return NL::BH1750::Error::ERROR_IIC_COMM;
	}
	bytes[1] = Wire.read();
	bytes[0] = Wire.read();
	return NL::BH1750::Error::OK;
}
