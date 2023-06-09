/**
 * @file MPU6050.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::MPU6050}.
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
#include "hardware/MPU6050.h"

bool NL::MPU6050::initialized = false;
uint8_t NL::MPU6050::deviceAddress;
NL::MPU6050::MPU6050AccScale NL::MPU6050::accScale;
NL::MPU6050::MPU6050GyScale NL::MPU6050::gyScale;

/**
 * @brief Start the MPU6050 motion sensor.
 * @param deviceAddress I²C address of the MPU6050 sensor
 * @return OK when the sensor was initialized
 * @return ERROR_IIC_COMM when the I²C communication failed
 */
NL::MPU6050::Error NL::MPU6050::begin(const uint8_t deviceAddress)
{
	return NL::MPU6050::begin(deviceAddress, NL::MPU6050::MPU6050AccScale::SCALE_2G, NL::MPU6050::MPU6050GyScale::SCALE_250DS);
}

/**
 * @brief Start the MPU6050 motion sensor.
 * @param deviceAddress I²C address of the MPU6050 sensor
 * @param accScale scale value, can be 2G, 4G, 8G, 16G
 * @param gyScale scale value, can be 200°/S, 500°/s, 1000°/s, 2000°/s
 * @return OK when the sensor was initialized
 * @return ERROR_IIC_COMM when the I²C communication failed
 */
NL::MPU6050::Error NL::MPU6050::begin(const uint8_t deviceAddress, const NL::MPU6050::MPU6050AccScale accScale, const NL::MPU6050::MPU6050GyScale gyScale)
{
	NL::MPU6050::initialized = false;
	NL::MPU6050::deviceAddress = deviceAddress;
	NL::MPU6050::accScale = accScale;
	NL::MPU6050::gyScale = gyScale;

	const NL::MPU6050::Error wakeError = NL::MPU6050::wake();
	if (wakeError != NL::MPU6050::Error::OK)
	{
		return NL::MPU6050::Error::ERROR_IIC_COMM;
	}

	const NL::MPU6050::Error accScaleError = NL::MPU6050::setAccScale(NL::MPU6050::accScale);
	if (accScaleError != NL::MPU6050::Error::OK)
	{
		return NL::MPU6050::Error::ERROR_IIC_COMM;
	}

	const NL::MPU6050::Error gyScaleError = NL::MPU6050::setGyScale(NL::MPU6050::gyScale);
	if (gyScaleError != NL::MPU6050::Error::OK)
	{
		return NL::MPU6050::Error::ERROR_IIC_COMM;
	}

	NL::MPU6050::initialized = true;
	return NL::MPU6050::Error::OK;
}

/**
 * @brief Stop the MPU6050 motion sensor.
 */
void NL::MPU6050::end()
{
	NL::MPU6050::initialized = false;
}

/**
 * @brief Return if the sensor was initialized correctly.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::MPU6050::isInitialized()
{
	return NL::MPU6050::initialized;
}

/**
 * @brief Wake up the MPU6050 motion sensor.
 * @return OK when the command was sent
 * @return ERROR_IIC_COMM when the communication failed
 */
NL::MPU6050::Error NL::MPU6050::wake()
{
	Wire.beginTransmission(NL::MPU6050::deviceAddress);
	Wire.write(0x6B);
	Wire.write(0B00000000);
	if (Wire.endTransmission(true) != 0)
	{
		return NL::MPU6050::Error::ERROR_IIC_COMM;
	}
	return NL::MPU6050::Error::OK;
}

/**
 * @brief Put the MPU6050 motion sensor into sleep mode.
 * @return OK when the command was sent
 * @return ERROR_IIC_COMM when the communication failed
 */
NL::MPU6050::Error NL::MPU6050::sleep()
{
	Wire.beginTransmission(NL::MPU6050::deviceAddress);
	Wire.write(0x6B);
	Wire.write(0B01000000);
	if (Wire.endTransmission(true) != 0)
	{
		return NL::MPU6050::Error::ERROR_IIC_COMM;
	}
	return NL::MPU6050::Error::OK;
}

/**
 * @brief Set the acc scale of the MPU6050.
 * @param accScale scale value, can be 2G, 4G, 8G, 16G
 * @return OK when the acc scale was updated
 * @return ERROR_IIC_COMM when the communication failed
 */
NL::MPU6050::Error NL::MPU6050::setAccScale(NL::MPU6050::MPU6050AccScale accScale)
{
	NL::MPU6050::accScale = accScale;
	Wire.beginTransmission(NL::MPU6050::deviceAddress);
	Wire.write(0x1C);
	Wire.write(static_cast<uint8_t>(NL::MPU6050::accScale));
	if (Wire.endTransmission(true) != 0)
	{
		return NL::MPU6050::Error::ERROR_IIC_COMM;
	}
	return NL::MPU6050::Error::OK;
}

/**
 * @brief Get the currently set acc scale of the MPU6050.
 * @return acc scale value
 */
NL::MPU6050::MPU6050AccScale NL::MPU6050::getAccScale()
{
	return NL::MPU6050::accScale;
}

/**
 * @brief Set the gyro scale of the MPU6050.
 * @param gyScale scale value, can be 200°/S, 500°/s, 1000°/s, 2000°/s
 * @return OK when the gyro scale was updated
 * @return ERROR_IIC_COMM when the communication failed
 */
NL::MPU6050::Error NL::MPU6050::setGyScale(NL::MPU6050::MPU6050GyScale gyScale)
{
	NL::MPU6050::gyScale = gyScale;
	Wire.beginTransmission(NL::MPU6050::deviceAddress);
	Wire.write(0x1B);
	Wire.write(static_cast<uint8_t>(NL::MPU6050::gyScale));
	if (Wire.endTransmission(true) != 0)
	{
		return NL::MPU6050::Error::ERROR_IIC_COMM;
	}
	return NL::MPU6050::Error::OK;
}

/**
 * @brief Get the currently set gyro scale of the MPU6050.
 * @return gyro scale value
 */
NL::MPU6050::MPU6050GyScale NL::MPU6050::getGyScale()
{
	return NL::MPU6050::gyScale;
}

/**
 * @brief Read the current sensor data.
 * @param motionData structure containing all data
 * @return OK when the data was received from the sensor
 * @return ERROR_IIC_COMM when the communication failed
 */
NL::MPU6050::Error NL::MPU6050::getData(NL::MPU6050::MPU6050MotionData &motionData)
{
	// Request the 6 acc registers
	Wire.beginTransmission(NL::MPU6050::deviceAddress);
	Wire.write(0x3B);
	if (Wire.endTransmission(false) != 0 || Wire.requestFrom(static_cast<int>(NL::MPU6050::deviceAddress), static_cast<int>(6), static_cast<int>(true)) != 6 || Wire.available() != 6)
	{
		return NL::MPU6050::Error::ERROR_IIC_COMM;
	}

	// Read the acc values
	motionData.accXRaw = (Wire.read() << 8 | Wire.read());
	motionData.accYRaw = (Wire.read() << 8 | Wire.read());
	motionData.accZRaw = (Wire.read() << 8 | Wire.read());

	// Request the 6 gyro registers
	Wire.beginTransmission(NL::MPU6050::deviceAddress);
	Wire.write(0x43);
	if (Wire.endTransmission(false) != 0 || Wire.requestFrom(static_cast<int>(NL::MPU6050::deviceAddress), static_cast<int>(6), static_cast<int>(true)) != 6 || Wire.available() != 6)
	{
		return NL::MPU6050::Error::ERROR_IIC_COMM;
	}

	// Read the rotation values
	motionData.gyroXRaw = (Wire.read() << 8 | Wire.read());
	motionData.gyroYRaw = (Wire.read() << 8 | Wire.read());
	motionData.gyroZRaw = (Wire.read() << 8 | Wire.read());

	// Request the 2 temperature registers
	Wire.beginTransmission(NL::MPU6050::deviceAddress);
	Wire.write(0x41);
	if (Wire.endTransmission(false) != 0 || Wire.requestFrom(static_cast<int>(NL::MPU6050::deviceAddress), static_cast<int>(2), static_cast<int>(true)) != 2 || Wire.available() != 2)
	{
		return NL::MPU6050::Error::ERROR_IIC_COMM;
	}

	// Read the temperature values
	motionData.temperatureRaw = (Wire.read() << 8 | Wire.read());

	// Calculate G values
	motionData.accXG = motionData.accXRaw / getScaleDiv(NL::MPU6050::accScale);
	motionData.accYG = motionData.accYRaw / getScaleDiv(NL::MPU6050::accScale);
	motionData.accZG = motionData.accZRaw / getScaleDiv(NL::MPU6050::accScale);

	// Calculate rotation value in degree/s
	motionData.gyroXDeg = motionData.gyroXRaw / getScaleDiv(NL::MPU6050::gyScale);
	motionData.gyroYDeg = motionData.gyroYRaw / getScaleDiv(NL::MPU6050::gyScale);
	motionData.gyroZDeg = motionData.gyroZRaw / getScaleDiv(NL::MPU6050::gyScale);

	// Calculate the temeprature from raw value
	motionData.temperatureDeg = motionData.temperatureRaw / 340.0f + 36.53f;

	return NL::MPU6050::Error::OK;
}

/**
 * @brief Get the divider to convert from raw value to G depending on the scale of the acc.
 * @param accScale scale value, can be 2G, 4G, 8G, 16G
 * @return float divider to convert from raw value to G
 */
float NL::MPU6050::getScaleDiv(const NL::MPU6050::MPU6050AccScale accScale)
{
	switch (accScale)
	{
	case NL::MPU6050::MPU6050AccScale::SCALE_2G:
		return 16383.5f;
	case NL::MPU6050::MPU6050AccScale::SCALE_4G:
		return 8191.75f;
	case NL::MPU6050::MPU6050AccScale::SCALE_8G:
		return 4095.875f;
	case NL::MPU6050::MPU6050AccScale::SCALE_16G:
		return 2047.9375f;
	default:
		return 1.0f;
	}
}

/**
 * @brief Get the divider to convert from raw value to degree/s depending on the scale of the acc.
 * @param gyScale  scale value, can be 200°/S, 500°/s, 1000°/s, 2000°/s
 * @return float divider to convert from raw value to degree/s
 */
float NL::MPU6050::getScaleDiv(const NL::MPU6050::MPU6050GyScale gyScale)
{
	switch (gyScale)
	{
	case NL::MPU6050::MPU6050GyScale::SCALE_250DS:
		return 131.068f;
	case NL::MPU6050::MPU6050GyScale::SCALE_500DS:
		return 65.534f;
	case NL::MPU6050::MPU6050GyScale::SCALE_1000DS:
		return 32.767f;
	case NL::MPU6050::MPU6050GyScale::SCALE_2000DS:
		return 16.3835f;
	default:
		return 1.0f;
	}
}
