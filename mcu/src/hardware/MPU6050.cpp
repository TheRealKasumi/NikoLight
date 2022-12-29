/**
 * @file MPU6050.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::MPU6050}.
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
#include "hardware/MPU6050.h"

/**
 * @brief Create a new instance of {@link TL::MPU6050}.
 * @param deviceAddress I²C address of the MPU6050 sensor
 */
TL::MPU6050::MPU6050(const uint8_t deviceAddress)
{
	this->deviceAddress = deviceAddress;
	this->accScale = TL::MPU6050::MPU6050AccScale::SCALE_2G;
	this->gyScale = TL::MPU6050::MPU6050GyScale::SCALE_250DS;
}

/**
 * @brief Create a new instance of {@link TL::MPU6050}.
 * @param deviceAddress I²C address of the MPU6050 sensor
 * @param accScale scale value, can be 2G, 4G, 8G, 16G
 * @param gyScale scale value, can be 200°/S, 500°/s, 1000°/s, 2000°/s
 */
TL::MPU6050::MPU6050(const uint8_t deviceAddress, const TL::MPU6050::MPU6050AccScale accScale, const TL::MPU6050::MPU6050GyScale gyScale)
{
	this->deviceAddress = deviceAddress;
	this->accScale = accScale;
	this->gyScale = gyScale;
}

/**
 * @brief Delete the {@link TL::MPU6050} instance and shut down the sensor.
 */
TL::MPU6050::~MPU6050()
{
	this->sleep();
}

/**
 * @brief Initialize the MPU6050 motion sensor.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::MPU6050::begin()
{
	if (!this->wake())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to wake MPU6050."));
		return false;
	}

	if (!this->setAccScale(this->accScale))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to wake MPU6050."));
		return false;
	}

	if (!this->setGyScale(this->gyScale))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to wake MPU6050."));
		return false;
	}

	return true;
}

/**
 * @brief Wake up the MPU6050 motion sensor.
 * @return true when successful
 * @return false when there was a communication error
 */
bool TL::MPU6050::wake()
{
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x6B);
	Wire.write(0B00000000);
	if (Wire.endTransmission(true) != 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to wake MPU6050."));
		return false;
	}
	return true;
}

/**
 * @brief Put the MPU6050 motion sensor into sleep mode.
 * @return true when successful
 * @return false when there was a communication error
 */
bool TL::MPU6050::sleep()
{
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x6B);
	Wire.write(0B01000000);
	if (Wire.endTransmission(true) != 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to put MPU6050 into sleep mode."));
		return false;
	}
	return true;
}

/**
 * @brief Set the acc scale of the MPU6050.
 * @param accScale scale value, can be 2G, 4G, 8G, 16G
 * @return true when successful
 * @return false when there was a communication error
 */
bool TL::MPU6050::setAccScale(TL::MPU6050::MPU6050AccScale accScale)
{
	this->accScale = accScale;
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x1C);
	Wire.write(this->accScale);
	if (Wire.endTransmission(true) != 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to set acceleration scale of MPU6050 sensor."));
		return false;
	}
	return true;
}

/**
 * @brief Get the currently set acc scale of the MPU6050.
 * @return acc scale value
 */
TL::MPU6050::MPU6050AccScale TL::MPU6050::getAccScale()
{
	return this->accScale;
}

/**
 * @brief Set the gyro scale of the MPU6050.
 * @param gyScale scale value, can be 200°/S, 500°/s, 1000°/s, 2000°/s
 * @return true when sucessful
 * @return false when there was a communication error
 */
bool TL::MPU6050::setGyScale(TL::MPU6050::MPU6050GyScale gyScale)
{
	this->gyScale = gyScale;
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x1B);
	Wire.write(this->gyScale);
	if (Wire.endTransmission(true) != 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to set gyro scale of MPU6050 sensor."));
		return false;
	}
	return true;
}

/**
 * @brief Get the currently set gyro scale of the MPU6050.
 * @return gyro scale value
 */
TL::MPU6050::MPU6050GyScale TL::MPU6050::getGyScale()
{
	return this->gyScale;
}

/**
 * @brief Read the current sensor data.
 * @param motionData structure containing all data
 * @return true when successful
 * @return false when there was an error
 */
bool TL::MPU6050::getData(TL::MPU6050::MPU6050MotionData &motionData)
{
	// Request the 6 acc registers
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x3B);
	if (Wire.endTransmission(false) != 0 || Wire.requestFrom(this->deviceAddress, 6, true) != 6 || Wire.available() != 6)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to request 6 acc registers of MPU6050 sensor."));
		return false;
	}

	// Read the acc values
	motionData.accXRaw = (Wire.read() << 8 | Wire.read());
	motionData.accYRaw = (Wire.read() << 8 | Wire.read());
	motionData.accZRaw = (Wire.read() << 8 | Wire.read());

	// Request the 6 gyro registers
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x43);
	if (Wire.endTransmission(false) != 0 || Wire.requestFrom(this->deviceAddress, 6, true) != 6 || Wire.available() != 6)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to request 6 gyro registers of MPU6050 sensor."));
		return false;
	}

	// Read the rotation values
	motionData.gyroXRaw = (Wire.read() << 8 | Wire.read());
	motionData.gyroYRaw = (Wire.read() << 8 | Wire.read());
	motionData.gyroZRaw = (Wire.read() << 8 | Wire.read());

	// Request the 2 temperature registers
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x41);
	if (Wire.endTransmission(false) != 0 || Wire.requestFrom(this->deviceAddress, 2, true) != 2 || Wire.available() != 2)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to request 2 temperature registers of MPU6050 sensor."));
		return false;
	}

	// Read the temperature values
	motionData.temperatureRaw = (Wire.read() << 8 | Wire.read());

	// Calculate G values
	motionData.accXG = motionData.accXRaw / getScaleDiv(this->accScale);
	motionData.accYG = motionData.accYRaw / getScaleDiv(this->accScale);
	motionData.accZG = motionData.accZRaw / getScaleDiv(this->accScale);

	// Calculate rotation value in degree/s
	motionData.gyroXDeg = motionData.gyroXRaw / getScaleDiv(this->gyScale);
	motionData.gyroYDeg = motionData.gyroYRaw / getScaleDiv(this->gyScale);
	motionData.gyroZDeg = motionData.gyroZRaw / getScaleDiv(this->gyScale);

	// Calculate the temeprature from raw value
	motionData.temperatureDeg = motionData.temperatureRaw / 340.0f + 36.53f;

	return true;
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
