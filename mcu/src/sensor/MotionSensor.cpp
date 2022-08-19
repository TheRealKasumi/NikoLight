/**
 * @file MotionSensor.cpp
 * @author TheRealKasumi
 * @brief implementation of the {@link TesLight::MotionSensor}.
 * @version 0.0.1
 * @date 2022-06-22
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "sensor/MotionSensor.h"

/**
 * @brief Construct a new instance of {@link TesLight::MotionSensor}.
 * @param deviceAddress I2C address of the MPU6050
 * @param sdaPin sda pin
 * @param sclPin scl pin
 * @param bufferSize size of the internal buffer
 */
TesLight::MotionSensor::MotionSensor(const uint8_t deviceAddress, const uint8_t sdaPin, const uint8_t sclPin, const uint8_t bufferSize)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("MotionSensor.cpp:MotionSensor"), F("Initializing Motion Sensor properties."));
	this->deviceAddress = deviceAddress;
	this->sdaPin = sdaPin;
	this->sclPin = sclPin;
	this->bufferSize = bufferSize > 0 ? bufferSize : 1;
	this->bufferIndex = 0;
	this->motionData = new TesLight::MotionSensorData[this->bufferSize];

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("MotionSensor.cpp:MotionSensor"), F("Initializing buffers."));
	this->initializeMotionSensorData(this->motionData, this->bufferSize);
	this->initializeMotionSensorData(&this->offsetData, 1);
}

/**
 * @brief Destroy the instance and free memory.
 */
TesLight::MotionSensor::~MotionSensor()
{
	delete[] this->motionData;
}

/**
 * @brief Initialize the MPU6050 and wake sit up. Set the acc range to +/- 2g and the gyro range to +/- 250°/s
 * @param autoCalibrate if set, autocalibration will run
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::MotionSensor::begin(const bool autoCalibrate)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("MotionSensor.cpp:begin"), F("Initializing Initializing Motion Sensor."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("MotionSensor.cpp:begin"), F("Initializing I2C communication."));
	Wire.begin(this->sdaPin, this->sclPin);

	// Wake up
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("MotionSensor.cpp:begin"), F("Waking up motion sensor."));
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x6B);
	Wire.write(0);
	if (Wire.endTransmission(true) != 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("MotionSensor.cpp:begin"), F("Communication error. Failed to wake motion sensor."));
		return false;
	}

	// Set acc to +/- 2G
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("MotionSensor.cpp:begin"), F("Setting acceleration scale to +/- 2G."));
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x1C);
	Wire.write(0x00);
	if (Wire.endTransmission(true) != 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("MotionSensor.cpp:begin"), F("Communication error. Failed to set acceleration scale."));
		return false;
	}

	// Set Gyro to +/- 250°/s
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("MotionSensor.cpp:begin"), F("Setting rotation scale to +/- 250°/s."));
	Wire.beginTransmission(this->deviceAddress);
	Wire.write(0x1B);
	Wire.write(0x00);
	if (Wire.endTransmission(true) != 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("MotionSensor.cpp:begin"), F("Communication error. Failed to set rotation scale."));
		return false;
	}

	// Initially fill the buffer
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("MotionSensor.cpp:begin"), F("Initially filling buffer."));
	if (!this->readData(false))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("MotionSensor.cpp:begin"), F("Communication error. Failed to read motion data."));
		return false;
	}

	// Set the offset for calibration
	if (autoCalibrate)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("MotionSensor.cpp:begin"), F("Calculating sensor offsets."));
		this->offsetData = this->getData();
		this->offsetData.accZRaw -= 16384;
		this->offsetData.accZG -= 1.0;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("MotionSensor.cpp:begin"), F("Motion sensor initialized successfully."));
	return true;
}

/**
 * @brief Read the motion data from the MPU6050.
 * @param asRingBuffer if set the buffer will be used as ring buffer
 * @return true when the data was read
 * @return false when there was an error
 */
bool TesLight::MotionSensor::readData(const bool asRingBuffer)
{
	for (uint8_t i = 0; i < this->bufferSize; i++)
	{
		// Read the 6 acc registers
		Wire.beginTransmission(this->deviceAddress);
		Wire.write(0x3B);
		Wire.endTransmission(false);
		if (Wire.requestFrom(this->deviceAddress, 6, true) != 6)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("MotionSensor.cpp:readData"), F("Communication error. Failed to read acceleration registers."));
			return false;
		}

		// Set and calculate the acc values
		this->motionData[this->bufferIndex].accXRaw = (Wire.read() << 8 | Wire.read());
		this->motionData[this->bufferIndex].accYRaw = (Wire.read() << 8 | Wire.read());
		this->motionData[this->bufferIndex].accZRaw = (Wire.read() << 8 | Wire.read());
		this->motionData[this->bufferIndex].accXG = this->motionData[this->bufferIndex].accXRaw / 16384.0;
		this->motionData[this->bufferIndex].accYG = this->motionData[this->bufferIndex].accYRaw / 16384.0;
		this->motionData[this->bufferIndex].accZG = this->motionData[this->bufferIndex].accZRaw / 16384.0;

		// Read the 6 gyro registers
		Wire.beginTransmission(this->deviceAddress);
		Wire.write(0x43);
		Wire.endTransmission(false);
		if (Wire.requestFrom(this->deviceAddress, 6, true) != 6)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("MotionSensor.cpp:readData"), F("Communication error. Failed to read rotation registers."));
			return false;
		}

		// Set and calculate the rotation values
		this->motionData[this->bufferIndex].gyroXRaw = (Wire.read() << 8 | Wire.read());
		this->motionData[this->bufferIndex].gyroYRaw = (Wire.read() << 8 | Wire.read());
		this->motionData[this->bufferIndex].gyroZRaw = (Wire.read() << 8 | Wire.read());
		this->motionData[this->bufferIndex].gyroXDeg = this->motionData[this->bufferIndex].gyroXRaw / 131.0;
		this->motionData[this->bufferIndex].gyroYDeg = this->motionData[this->bufferIndex].gyroYRaw / 131.0;
		this->motionData[this->bufferIndex].gyroZDeg = this->motionData[this->bufferIndex].gyroZRaw / 131.0;

		// Handle the buffer index
		this->bufferIndex++;
		if (this->bufferIndex == this->bufferSize)
		{
			this->bufferIndex = 0;
		}

		// When used as ring buffer, stop after one iteration
		if (asRingBuffer)
		{
			break;
		}
	}

	return true;
}

/**
 * @brief Get the current motion data respecting the calibration data.
 * @return {@link TesLight::MotionSensorData} containing the motion data
 */
TesLight::MotionSensorData TesLight::MotionSensor::getData()
{
	TesLight::MotionSensorData data = this->average(this->motionData, this->bufferSize);
	data.accXRaw -= this->offsetData.accXRaw;
	data.accYRaw -= this->offsetData.accYRaw;
	data.accZRaw -= this->offsetData.accZRaw;
	data.gyroXRaw -= this->offsetData.gyroXRaw;
	data.gyroYRaw -= this->offsetData.gyroYRaw;
	data.gyroZRaw -= this->offsetData.gyroZRaw;
	data.accXG -= this->offsetData.accXG;
	data.accYG -= this->offsetData.accYG;
	data.accZG -= this->offsetData.accZG;
	data.gyroXDeg -= this->offsetData.gyroXDeg;
	data.gyroYDeg -= this->offsetData.gyroYDeg;
	data.gyroZDeg -= this->offsetData.gyroZDeg;
	return data;
}

/**
 * @brief Set the offset data for sensor calibration.
 * @param offsetData offset data
 */
void TesLight::MotionSensor::setOffsetData(TesLight::MotionSensorData offsetData)
{
	this->offsetData = offsetData;
}

/**
 * @brief Get the calculate offset data for sensor calibration.
 * @return instance of {@link TesLight::MotionSensorData}
 */
TesLight::MotionSensorData TesLight::MotionSensor::getOffsetData()
{
	return this->offsetData;
}

/**
 * @brief Initialize {@link TesLight::MotionSensorData} to 0 values.
 * @param data pointer to the instance
 * @param sampleSize number of samples to initialize
 */
void TesLight::MotionSensor::initializeMotionSensorData(TesLight::MotionSensorData *data, const uint8_t sampleSize)
{
	for (uint8_t i = 0; i < sampleSize; i++)
	{
		data[i].accXRaw = 0;
		data[i].accYRaw = 0;
		data[i].accZRaw = 0;
		data[i].gyroXRaw = 0;
		data[i].gyroYRaw = 0;
		data[i].gyroZRaw = 0;
		data[i].accXG = 0.0;
		data[i].accYG = 0.0;
		data[i].accZG = 0.0;
		data[i].gyroXDeg = 0.0;
		data[i].gyroYDeg = 0.0;
		data[i].gyroZDeg = 0.0;
	}
}

/**
 * @brief Average buffer of {@link TesLight::MotionSensorData}.
 * @param data buffer containing the data
 * @param sampleSize number of samples
 * @return {@link TesLight::MotionSensorData} containing the average values
 */
TesLight::MotionSensorData TesLight::MotionSensor::average(TesLight::MotionSensorData *data, const uint8_t sampleSize)
{
	double accXRaw = 0.0;
	double accYRaw = 0.0;
	double accZRaw = 0.0;
	double gyroXRaw = 0.0;
	double gyroYRaw = 0.0;
	double gyroZRaw = 0.0;
	double accXG = 0.0;
	double accYG = 0.0;
	double accZG = 0.0;
	double gyroXDeg = 0.0;
	double gyroYDeg = 0.0;
	double gyroZDeg = 0.0;

	for (uint8_t i = 0; i < sampleSize; i++)
	{
		accXRaw += data[i].accXRaw * (1.0 / sampleSize);
		accYRaw += data[i].accYRaw * (1.0 / sampleSize);
		accZRaw += data[i].accZRaw * (1.0 / sampleSize);
		gyroXRaw += data[i].gyroXRaw * (1.0 / sampleSize);
		gyroYRaw += data[i].gyroYRaw * (1.0 / sampleSize);
		gyroZRaw += data[i].gyroZRaw * (1.0 / sampleSize);
		accXG += data[i].accXG * (1.0 / sampleSize);
		accYG += data[i].accYG * (1.0 / sampleSize);
		accZG += data[i].accZG * (1.0 / sampleSize);
		gyroXDeg += data[i].gyroXDeg * (1.0 / sampleSize);
		gyroYDeg += data[i].gyroYDeg * (1.0 / sampleSize);
		gyroZDeg += data[i].gyroZDeg * (1.0 / sampleSize);
	}

	TesLight::MotionSensorData averageData;
	averageData.accXRaw = (int16_t)accXRaw;
	averageData.accYRaw = (int16_t)accYRaw;
	averageData.accZRaw = (int16_t)accZRaw;
	averageData.gyroXRaw = (int16_t)gyroXRaw;
	averageData.gyroYRaw = (int16_t)gyroYRaw;
	averageData.gyroZRaw = (int16_t)gyroZRaw;
	averageData.accXG = accXG;
	averageData.accYG = accYG;
	averageData.accZG = accZG;
	averageData.gyroXDeg = gyroXDeg;
	averageData.gyroYDeg = gyroYDeg;
	averageData.gyroZDeg = gyroZDeg;

	return averageData;
}