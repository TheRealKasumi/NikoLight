/**
 * @file MPU6050.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::MPU6050}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "hardware/MPU6050.h"

/**
 * @brief Create a new instance of {@link TesLight::MPU6050}.
 * @param deviceAddress I²C address of the MPU6050 sensor
 */
TesLight::MPU6050::MPU6050(const uint8_t deviceAddress)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize MPU6050 instance."));
	this->deviceAddress = deviceAddress;
	this->accScale = TesLight::MPU6050::MPU6050AccScale::SCALE_2G;
	this->gyScale = TesLight::MPU6050::MPU6050GyScale::SCALE_250DS;
}

/**
 * @brief Create a new instance of {@link TesLight::MPU6050}.
 * @param deviceAddress I²C address of the MPU6050 sensor
 * @param accScale scale value, can be 2G, 4G, 8G, 16G
 * @param gyScale scale value, can be 200°/S, 500°/s, 1000°/s, 2000°/s
 */
TesLight::MPU6050::MPU6050(const uint8_t deviceAddress, const TesLight::MPU6050::MPU6050AccScale accScale, const TesLight::MPU6050::MPU6050GyScale gyScale)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Delete DS18B20 instance and free resources."));
	this->deviceAddress = deviceAddress;
	this->accScale = accScale;
	this->gyScale = gyScale;
}

/**
 * @brief Delete the {@link TesLight::MPU6050} instance and shut down the sensor.
 */
TesLight::MPU6050::~MPU6050()
{
	this->sleep();
}

/**
 * @brief Initialize the MPU6050 motion sensor.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::MPU6050::begin()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initializing MPU6050 sensor."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Wake up sensor."));
	if (!this->wake())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to wake MPU6050."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set acc scale."));
	if (!this->setAccScale(this->accScale))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to wake MPU6050."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set gyro scale."));
	if (!this->setGyScale(this->gyScale))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to wake MPU6050."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("MPU6050 initialized."));
	return true;
}

/**
 * @brief Wake up the MPU6050 motion sensor.
 * @return true when successful
 * @return false when there was a communication error
 */
bool TesLight::MPU6050::wake()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Wake up MPU6050 sensor."));
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x6B);
	Wire.write(0B00000000);
	if (Wire.endTransmission(true) != 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to wake MPU6050."));
		return false;
	}
	return true;
}

/**
 * @brief Put the MPU6050 motion sensor into sleep mode.
 * @return true when successful
 * @return false when there was a communication error
 */
bool TesLight::MPU6050::sleep()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Put MPU6050 sensor into sleep mode."));
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x6B);
	Wire.write(0B01000000);
	if (Wire.endTransmission(true) != 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to put MPU6050 into sleep mode."));
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
bool TesLight::MPU6050::setAccScale(TesLight::MPU6050::MPU6050AccScale accScale)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set acceleration scale of the MPU6050 sensor."));
	this->accScale = accScale;
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x1C);
	Wire.write(this->accScale);
	if (Wire.endTransmission(true) != 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to set acceleration scale of MPU6050 sensor."));
		return false;
	}
	return true;
}

/**
 * @brief Get the currently set acc scale of the MPU6050.
 * @return acc scale value
 */
TesLight::MPU6050::MPU6050AccScale TesLight::MPU6050::getAccScale()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get acc scale of MPU6050 sensor."));
	return this->accScale;
}

/**
 * @brief Set the gyro scale of the MPU6050.
 * @param gyScale scale value, can be 200°/S, 500°/s, 1000°/s, 2000°/s
 * @return true when sucessful
 * @return false when there was a communication error
 */
bool TesLight::MPU6050::setGyScale(TesLight::MPU6050::MPU6050GyScale gyScale)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set gyro scale of the MPU6050 sensor."));
	this->gyScale = gyScale;
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x1B);
	Wire.write(this->gyScale);
	if (Wire.endTransmission(true) != 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to set gyro scale of MPU6050 sensor."));
		return false;
	}
	return true;
}

/**
 * @brief Get the currently set gyro scale of the MPU6050.
 * @return gyro scale value
 */
TesLight::MPU6050::MPU6050GyScale TesLight::MPU6050::getGyScale()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Get gyro scale of MPU6050 sensor."));
	return this->gyScale;
}

/**
 * @brief Read the current sensor data.
 * @param motionData structure containing all data
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::MPU6050::getData(TesLight::MPU6050::MPU6050MotionData &motionData)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Reading data registers of the MPU6050 sensor."));

	// Request the 6 acc registers
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x3B);
	if (Wire.endTransmission(false) != 0 || Wire.requestFrom(this->deviceAddress, 6, true) != 6 || Wire.available() != 6)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to request 6 acc registers of MPU6050 sensor."));
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
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to request 6 gyro registers of MPU6050 sensor."));
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
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("I²C communication error. Failed to request 2 temperature registers of MPU6050 sensor."));
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
float TesLight::MPU6050::getScaleDiv(const TesLight::MPU6050::MPU6050AccScale accScale)
{
	switch (accScale)
	{
	case TesLight::MPU6050::MPU6050AccScale::SCALE_2G:
		return 16383.5f;
	case TesLight::MPU6050::MPU6050AccScale::SCALE_4G:
		return 8191.75f;
	case TesLight::MPU6050::MPU6050AccScale::SCALE_8G:
		return 4095.875f;
	case TesLight::MPU6050::MPU6050AccScale::SCALE_16G:
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
float TesLight::MPU6050::getScaleDiv(const TesLight::MPU6050::MPU6050GyScale gyScale)
{
	switch (gyScale)
	{
	case TesLight::MPU6050::MPU6050GyScale::SCALE_250DS:
		return 131.068f;
	case TesLight::MPU6050::MPU6050GyScale::SCALE_500DS:
		return 65.534f;
	case TesLight::MPU6050::MPU6050GyScale::SCALE_1000DS:
		return 32.767f;
	case TesLight::MPU6050::MPU6050GyScale::SCALE_2000DS:
		return 16.3835f;
	default:
		return 1.0f;
	}
}
