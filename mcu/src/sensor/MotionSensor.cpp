/**
 * @file motionSensor.cpp
 * @author TheRealKasumi
 * @brief implementation of the {@link TL::MotionSensor}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "sensor/MotionSensor.h"

/**
 * @brief Create a new instance of {@link TL::MotionSensor}.
 * @param sensorAddress address of the sensor on the I²C bus
 * @param configuration configuration of TesLight
 */
TL::MotionSensor::MotionSensor(const uint8_t sensorAddress, TL::Configuration *configuration)
{
	this->mpu6050.reset(new TL::MPU6050(sensorAddress));
	this->configuration = configuration;
	this->motionData.accXRaw = 0;
	this->motionData.accYRaw = 0;
	this->motionData.accZRaw = 0;
	this->motionData.gyroXRaw = 0;
	this->motionData.gyroYRaw = 0;
	this->motionData.gyroZRaw = 0;
	this->motionData.accXG = 0.0f;
	this->motionData.accYG = 0.0f;
	this->motionData.accZG = 0.0f;
	this->motionData.gyroXDeg = 0.0f;
	this->motionData.gyroYDeg = 0.0f;
	this->motionData.gyroZDeg = 0.0f;
	this->motionData.pitch = 0.0f;
	this->motionData.roll = 0.0f;
	this->motionData.yaw = 0.0f;
	this->motionData.rollCompensatedAccXG = 0.0f;
	this->motionData.pitchCompensatedAccYG = 0.0f;
	this->motionData.temperatureRaw = 0;
	this->motionData.temperatureDeg = 0;
	this->lastMeasure = 0;
}

/**
 * @brief Delete the {@link TL::MotionSensor} instance.
 */
TL::MotionSensor::~MotionSensor()
{
}

/**
 * @brief Initialize the motion sensor and set the scales.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::MotionSensor::begin()
{
	if (!this->mpu6050->begin())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start MPU6050 sensor."));
		return false;
	}

	if (!this->mpu6050->setAccScale(TL::MPU6050::MPU6050AccScale::SCALE_4G))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to set acc scale of MPU6050 sensor."));
		return false;
	}

	if (!this->mpu6050->setGyScale(TL::MPU6050::MPU6050GyScale::SCALE_500DS))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to set gyro scale of MPU6050 sensor."));
		return false;
	}

	return true;
}

/**
 * @brief Run the measurement and calculation cycle.
 * @return true when successful
 * @return false when there was an error
 */
bool TL::MotionSensor::run()
{
	TL::MPU6050::MPU6050MotionData sensorData;
	if (!this->mpu6050->getData(sensorData))
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read sensor data from MPU6050."));
		return false;
	}

	const TL::Configuration::MotionSensorCalibration calibrationData = this->configuration->getMotionSensorCalibration();
	const unsigned long timeStep = this->lastMeasure == 0 ? 0.0f : (micros() - this->lastMeasure);
	const float timeScale = timeStep / 1000000.0f;
	this->lastMeasure = micros();

	this->motionData.accXRaw = sensorData.accXRaw - calibrationData.accXRaw;
	this->motionData.accYRaw = sensorData.accYRaw - calibrationData.accYRaw;
	this->motionData.accZRaw = sensorData.accZRaw - calibrationData.accZRaw;
	this->motionData.gyroXRaw = sensorData.gyroXRaw - calibrationData.gyroXRaw;
	this->motionData.gyroYRaw = sensorData.gyroYRaw - calibrationData.gyroYRaw;
	this->motionData.gyroZRaw = sensorData.gyroZRaw - calibrationData.gyroZRaw;
	this->motionData.accXG = sensorData.accXG - calibrationData.accXG;
	this->motionData.accYG = sensorData.accYG - calibrationData.accYG;
	this->motionData.accZG = sensorData.accZG - calibrationData.accZG;
	this->motionData.gyroXDeg = sensorData.gyroXDeg - calibrationData.gyroXDeg;
	this->motionData.gyroYDeg = sensorData.gyroYDeg - calibrationData.gyroYDeg;
	this->motionData.gyroZDeg = sensorData.gyroZDeg - calibrationData.gyroZDeg;
	this->motionData.temperatureRaw = sensorData.temperatureRaw;
	this->motionData.temperatureDeg = sensorData.temperatureDeg;

	this->motionData.pitch += this->motionData.gyroXDeg * timeScale;
	this->motionData.roll += this->motionData.gyroYDeg * timeScale;
	this->motionData.yaw += this->motionData.gyroZDeg * timeScale;

	const float accPitch = atan(this->motionData.accYG / this->motionData.accZG) * 180.0f / PI;
	this->motionData.pitch += (accPitch - this->motionData.pitch) / 500.0f;

	const float accRoll = -atan(this->motionData.accXG / this->motionData.accZG) * 180.0f / PI;
	this->motionData.roll += (accRoll - this->motionData.roll) / 500.0f;

	this->motionData.rollCompensatedAccXG = this->motionData.accXG + sin(this->motionData.roll / 180.0f * PI);
	this->motionData.pitchCompensatedAccYG = this->motionData.accYG - sin(this->motionData.pitch / 180.0f * PI);

	return true;
}

/**
 * @brief Calibrate the motion sensor.
 * @param failOnTemperature if set to true, the calibration will fail when the sensor is too cold or warm
 * @return 0 when successful
 * @return 1 when there is a communication error
 * @return 2 when the motion sensor is too cold
 * @return 3 when the motion sensor is too warm
 */
uint8_t TL::MotionSensor::calibrate(const bool failOnTemperature)
{
	if (failOnTemperature)
	{
		TL::MPU6050::MPU6050MotionData sensorData;
		if (!this->mpu6050->getData(sensorData))
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read sensor data from MPU6050."));
			return 1;
		}

		if (sensorData.temperatureDeg < 20.0f)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor becaues the temperature is too low."));
			return 2;
		}
		else if (sensorData.temperatureDeg > 40.0f)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor becaues the temperature is too high."));
			return 3;
		}
	}

	double calibrationData[12] = {0.0f};
	for (uint16_t i = 0; i < 1000; i++)
	{
		TL::MPU6050::MPU6050MotionData sensorData;
		if (!this->mpu6050->getData(sensorData))
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read sensor data from MPU6050."));
			return 1;
		}

		calibrationData[0] += sensorData.accXRaw / 1000.0f;
		calibrationData[1] += sensorData.accYRaw / 1000.0f;
		calibrationData[2] += sensorData.accZRaw / 1000.0f;
		calibrationData[3] += sensorData.gyroXRaw / 1000.0f;
		calibrationData[4] += sensorData.gyroYRaw / 1000.0f;
		calibrationData[5] += sensorData.gyroZRaw / 1000.0f;
		calibrationData[6] += sensorData.accXG / 1000.0f;
		calibrationData[7] += sensorData.accYG / 1000.0f;
		calibrationData[8] += sensorData.accZG / 1000.0f;
		calibrationData[9] += sensorData.gyroXDeg / 1000.0f;
		calibrationData[10] += sensorData.gyroYDeg / 1000.0f;
		calibrationData[11] += sensorData.gyroZDeg / 1000.0f;
	}

	TL::Configuration::MotionSensorCalibration calibration = this->configuration->getMotionSensorCalibration();
	calibration.accXRaw = calibrationData[0];
	calibration.accYRaw = calibrationData[1];
	// calibration.accZRaw = calibrationData[2];
	calibration.gyroXRaw = calibrationData[3];
	calibration.gyroYRaw = calibrationData[4];
	calibration.gyroZRaw = calibrationData[5];
	calibration.accXG = calibrationData[6];
	calibration.accYG = calibrationData[7];
	// calibration.accZG = calibrationData[8];
	calibration.gyroXDeg = calibrationData[9];
	calibration.gyroYDeg = calibrationData[10];
	calibration.gyroZDeg = calibrationData[11];
	this->configuration->setMotionSensorCalibration(calibration);

	return 0;
}

/**
 * @brief Get the current motion data.
 * @return full set of motion data
 */
TL::MotionSensor::MotionSensorData TL::MotionSensor::getMotion()
{
	return this->motionData;
}
