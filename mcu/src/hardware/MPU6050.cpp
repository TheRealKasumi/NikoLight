/**
 * @file MPU6050.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::MPU6050}.
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
#include "hardware/MPU6050.h"

bool TL::MPU6050::initialized = false;
uint8_t TL::MPU6050::deviceAddress;
TL::MPU6050::MPU6050AccScale TL::MPU6050::accScale;
TL::MPU6050::MPU6050GyScale TL::MPU6050::gyScale;

/**
 * @brief Start the MPU6050 motion sensor.
 * @param deviceAddress I²C address of the MPU6050 sensor
 * @return OK when the sensor was initialized
 * @return ERROR_IIC_COMM when the I²C communication failed
 */
TL::MPU6050::Error TL::MPU6050::begin(const uint8_t deviceAddress)
{
	return TL::MPU6050::begin(deviceAddress, TL::MPU6050::MPU6050AccScale::SCALE_2G, TL::MPU6050::MPU6050GyScale::SCALE_250DS);
}

/**
 * @brief Start the MPU6050 motion sensor.
 * @param deviceAddress I²C address of the MPU6050 sensor
 * @param accScale scale value, can be 2G, 4G, 8G, 16G
 * @param gyScale scale value, can be 200°/S, 500°/s, 1000°/s, 2000°/s
 * @return OK when the sensor was initialized
 * @return ERROR_IIC_COMM when the I²C communication failed
 */
TL::MPU6050::Error TL::MPU6050::begin(const uint8_t deviceAddress, const TL::MPU6050::MPU6050AccScale accScale, const TL::MPU6050::MPU6050GyScale gyScale)
{
	TL::MPU6050::initialized = false;
	TL::MPU6050::deviceAddress = deviceAddress;
	TL::MPU6050::accScale = accScale;
	TL::MPU6050::gyScale = gyScale;

	const TL::MPU6050::Error wakeError = TL::MPU6050::wake();
	if (wakeError != TL::MPU6050::Error::OK)
	{
		return TL::MPU6050::Error::ERROR_IIC_COMM;
	}

	const TL::MPU6050::Error accScaleError = TL::MPU6050::setAccScale(TL::MPU6050::accScale);
	if (accScaleError != TL::MPU6050::Error::OK)
	{
		return TL::MPU6050::Error::ERROR_IIC_COMM;
	}

	const TL::MPU6050::Error gyScaleError = TL::MPU6050::setGyScale(TL::MPU6050::gyScale);
	if (gyScaleError != TL::MPU6050::Error::OK)
	{
		return TL::MPU6050::Error::ERROR_IIC_COMM;
	}

	TL::MPU6050::initialized = true;
	return TL::MPU6050::Error::OK;
}

/**
 * @brief Stop the MPU6050 motion sensor.
 */
void TL::MPU6050::end()
{
	TL::MPU6050::initialized = false;
}

/**
 * @brief Return if the sensor was initialized correctly.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::MPU6050::isInitialized()
{
	return TL::MPU6050::initialized;
}

/**
 * @brief Wake up the MPU6050 motion sensor.
 * @return OK when the command was sent
 * @return ERROR_IIC_COMM when the communication failed
 */
TL::MPU6050::Error TL::MPU6050::wake()
{
	Wire.beginTransmission(TL::MPU6050::deviceAddress);
	Wire.write(0x6B);
	Wire.write(0B00000000);
	if (Wire.endTransmission(true) != 0)
	{
		return TL::MPU6050::Error::ERROR_IIC_COMM;
	}
	return TL::MPU6050::Error::OK;
}

/**
 * @brief Put the MPU6050 motion sensor into sleep mode.
 * @return OK when the command was sent
 * @return ERROR_IIC_COMM when the communication failed
 */
TL::MPU6050::Error TL::MPU6050::sleep()
{
	Wire.beginTransmission(TL::MPU6050::deviceAddress);
	Wire.write(0x6B);
	Wire.write(0B01000000);
	if (Wire.endTransmission(true) != 0)
	{
		return TL::MPU6050::Error::ERROR_IIC_COMM;
	}
	return TL::MPU6050::Error::OK;
}

/**
 * @brief Set the acc scale of the MPU6050.
 * @param accScale scale value, can be 2G, 4G, 8G, 16G
 * @return OK when the acc scale was updated
 * @return ERROR_IIC_COMM when the communication failed
 */
TL::MPU6050::Error TL::MPU6050::setAccScale(TL::MPU6050::MPU6050AccScale accScale)
{
	TL::MPU6050::accScale = accScale;
	Wire.beginTransmission(TL::MPU6050::deviceAddress);
	Wire.write(0x1C);
	Wire.write(TL::MPU6050::accScale);
	if (Wire.endTransmission(true) != 0)
	{
		return TL::MPU6050::Error::ERROR_IIC_COMM;
	}
	return TL::MPU6050::Error::OK;
}

/**
 * @brief Get the currently set acc scale of the MPU6050.
 * @return acc scale value
 */
TL::MPU6050::MPU6050AccScale TL::MPU6050::getAccScale()
{
	return TL::MPU6050::accScale;
}

/**
 * @brief Set the gyro scale of the MPU6050.
 * @param gyScale scale value, can be 200°/S, 500°/s, 1000°/s, 2000°/s
 * @return OK when the gyro scale was updated
 * @return ERROR_IIC_COMM when the communication failed
 */
TL::MPU6050::Error TL::MPU6050::setGyScale(TL::MPU6050::MPU6050GyScale gyScale)
{
	TL::MPU6050::gyScale = gyScale;
	Wire.beginTransmission(TL::MPU6050::deviceAddress);
	Wire.write(0x1B);
	Wire.write(TL::MPU6050::gyScale);
	if (Wire.endTransmission(true) != 0)
	{
		return TL::MPU6050::Error::ERROR_IIC_COMM;
	}
	return TL::MPU6050::Error::OK;
}

/**
 * @brief Get the currently set gyro scale of the MPU6050.
 * @return gyro scale value
 */
TL::MPU6050::MPU6050GyScale TL::MPU6050::getGyScale()
{
	return TL::MPU6050::gyScale;
}

/**
 * @brief Read the current sensor data.
 * @param motionData structure containing all data
 * @return OK when the data was received from the sensor
 * @return ERROR_IIC_COMM when the communication failed
 */
TL::MPU6050::Error TL::MPU6050::getData(TL::MPU6050::MPU6050MotionData &motionData)
{
	// Request the 6 acc registers
	Wire.beginTransmission(TL::MPU6050::deviceAddress);
	Wire.write(0x3B);
	if (Wire.endTransmission(false) != 0 || Wire.requestFrom(static_cast<int>(TL::MPU6050::deviceAddress), static_cast<int>(6), static_cast<int>(true)) != 6 || Wire.available() != 6)
	{
		return TL::MPU6050::Error::ERROR_IIC_COMM;
	}

	// Read the acc values
	motionData.accXRaw = (Wire.read() << 8 | Wire.read());
	motionData.accYRaw = (Wire.read() << 8 | Wire.read());
	motionData.accZRaw = (Wire.read() << 8 | Wire.read());

	// Request the 6 gyro registers
	Wire.beginTransmission(TL::MPU6050::deviceAddress);
	Wire.write(0x43);
	if (Wire.endTransmission(false) != 0 || Wire.requestFrom(static_cast<int>(TL::MPU6050::deviceAddress), static_cast<int>(6), static_cast<int>(true)) != 6 || Wire.available() != 6)
	{
		return TL::MPU6050::Error::ERROR_IIC_COMM;
	}

	// Read the rotation values
	motionData.gyroXRaw = (Wire.read() << 8 | Wire.read());
	motionData.gyroYRaw = (Wire.read() << 8 | Wire.read());
	motionData.gyroZRaw = (Wire.read() << 8 | Wire.read());

	// Request the 2 temperature registers
	Wire.beginTransmission(TL::MPU6050::deviceAddress);
	Wire.write(0x41);
	if (Wire.endTransmission(false) != 0 || Wire.requestFrom(static_cast<int>(TL::MPU6050::deviceAddress), static_cast<int>(2), static_cast<int>(true)) != 2 || Wire.available() != 2)
	{
		return TL::MPU6050::Error::ERROR_IIC_COMM;
	}

	// Read the temperature values
	motionData.temperatureRaw = (Wire.read() << 8 | Wire.read());

	// Calculate G values
	motionData.accXG = motionData.accXRaw / getScaleDiv(TL::MPU6050::accScale);
	motionData.accYG = motionData.accYRaw / getScaleDiv(TL::MPU6050::accScale);
	motionData.accZG = motionData.accZRaw / getScaleDiv(TL::MPU6050::accScale);

	// Calculate rotation value in degree/s
	motionData.gyroXDeg = motionData.gyroXRaw / getScaleDiv(TL::MPU6050::gyScale);
	motionData.gyroYDeg = motionData.gyroYRaw / getScaleDiv(TL::MPU6050::gyScale);
	motionData.gyroZDeg = motionData.gyroZRaw / getScaleDiv(TL::MPU6050::gyScale);

	// Calculate the temeprature from raw value
	motionData.temperatureDeg = motionData.temperatureRaw / 340.0f + 36.53f;

	return TL::MPU6050::Error::OK;
}

/**
 * @brief Get the divider to convert from raw value to G depending on the scale of the acc.
 * @param accScale scale value, can be 2G, 4G, 8G, 16G
 * @return float divider to convert from raw value to G
 */
float TL::MPU6050::getScaleDiv(const TL::MPU6050::MPU6050AccScale accScale)
{
	switch (accScale)
	{
	case TL::MPU6050::MPU6050AccScale::SCALE_2G:
		return 16383.5f;
	case TL::MPU6050::MPU6050AccScale::SCALE_4G:
		return 8191.75f;
	case TL::MPU6050::MPU6050AccScale::SCALE_8G:
		return 4095.875f;
	case TL::MPU6050::MPU6050AccScale::SCALE_16G:
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
float TL::MPU6050::getScaleDiv(const TL::MPU6050::MPU6050GyScale gyScale)
{
	switch (gyScale)
	{
	case TL::MPU6050::MPU6050GyScale::SCALE_250DS:
		return 131.068f;
	case TL::MPU6050::MPU6050GyScale::SCALE_500DS:
		return 65.534f;
	case TL::MPU6050::MPU6050GyScale::SCALE_1000DS:
		return 32.767f;
	case TL::MPU6050::MPU6050GyScale::SCALE_2000DS:
		return 16.3835f;
	default:
		return 1.0f;
	}
}
