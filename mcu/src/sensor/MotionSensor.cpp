/**
 * @file motionSensor.cpp
 * @author TheRealKasumi
 * @brief implementation of the {@link TesLight::MotionSensor}.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "sensor/MotionSensor.h"

/**
 * @brief Create a new instance of {@link TesLight::MotionSensor}.
 * @param sensorAddress address of the sensor on the I²C bus
 */
TesLight::MotionSensor::MotionSensor(const uint8_t sensorAddress)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize motion sensor."));
	this->mpu6050 = new TesLight::MPU6050(sensorAddress);
	this->calibrationData.accXRaw = 0;
	this->calibrationData.accYRaw = 0;
	this->calibrationData.accZRaw = 0;
	this->calibrationData.gyroXRaw = 0;
	this->calibrationData.gyroYRaw = 0;
	this->calibrationData.gyroZRaw = 0;
	this->calibrationData.accXG = 0.0f;
	this->calibrationData.accYG = 0.0f;
	this->calibrationData.accZG = 0.0f;
	this->calibrationData.gyroXDeg = 0.0f;
	this->calibrationData.gyroYDeg = 0.0f;
	this->calibrationData.gyroZDeg = 0.0f;
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
 * @brief Delete the {@link TesLight::MotionSensor} instance and free memory.
 */
TesLight::MotionSensor::~MotionSensor()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Delete motion sensor and free resources."));
	delete this->mpu6050;
	this->mpu6050 = nullptr;
}

/**
 * @brief Initialize the motion sensor and set the scales.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::MotionSensor::begin()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Start motion sensor."));
	if (!this->mpu6050->begin())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start MPU6050 sensor."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set acc scale to 4g."));
	if (!this->mpu6050->setAccScale(TesLight::MPU6050::MPU6050AccScale::SCALE_4G))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to set acc scale of MPU6050 sensor."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set gyro scale to 500°/s."));
	if (!this->mpu6050->setGyScale(TesLight::MPU6050::MPU6050GyScale::SCALE_500DS))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to set gyro scale of MPU6050 sensor."));
		return false;
	}

	return true;
}

/**
 * @brief Run the measurement and calculation cycle.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::MotionSensor::run()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Run motion measurement and orientation calculation of the motion sensor."));
	TesLight::MPU6050::MPU6050MotionData sensorData;
	if (!this->mpu6050->getData(sensorData))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read sensor data from MPU6050."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calculate the time scale."));
	const unsigned long timeStep = this->lastMeasure == 0 ? 0.0f : (micros() - this->lastMeasure);
	const float timeScale = timeStep / 1000000.0f;
	this->lastMeasure = micros();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set basic motion data."));
	this->motionData.accXRaw = sensorData.accXRaw - this->calibrationData.accXRaw;
	this->motionData.accYRaw = sensorData.accYRaw - this->calibrationData.accYRaw;
	this->motionData.accZRaw = sensorData.accZRaw - this->calibrationData.accZRaw;
	this->motionData.gyroXRaw = sensorData.gyroXRaw - this->calibrationData.gyroXRaw;
	this->motionData.gyroYRaw = sensorData.gyroYRaw - this->calibrationData.gyroYRaw;
	this->motionData.gyroZRaw = sensorData.gyroZRaw - this->calibrationData.gyroZRaw;
	this->motionData.accXG = sensorData.accXG - this->calibrationData.accXG;
	this->motionData.accYG = sensorData.accYG - this->calibrationData.accYG;
	this->motionData.accZG = sensorData.accZG - this->calibrationData.accZG;
	this->motionData.gyroXDeg = sensorData.gyroXDeg - this->calibrationData.gyroXDeg;
	this->motionData.gyroYDeg = sensorData.gyroYDeg - this->calibrationData.gyroYDeg;
	this->motionData.gyroZDeg = sensorData.gyroZDeg - this->calibrationData.gyroZDeg;
	this->motionData.temperatureRaw = sensorData.temperatureRaw;
	this->motionData.temperatureDeg = sensorData.temperatureDeg;

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Tracking rotation using gyros."));
	this->motionData.pitch += this->motionData.gyroXDeg * timeScale;
	this->motionData.roll += this->motionData.gyroYDeg * timeScale;
	this->motionData.yaw += this->motionData.gyroZDeg * timeScale;

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Apply drift compensation for pitch axis using accelerometer."));
	const float accPitch = atan(this->motionData.accYG / this->motionData.accZG) * 180.0f / PI;
	this->motionData.pitch += (accPitch - this->motionData.pitch) / 500.0f;

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Apply drift compensation for roll axis using accelerometer."));
	const float accRoll = -atan(this->motionData.accXG / this->motionData.accZG) * 180.0f / PI;
	this->motionData.roll += (accRoll - this->motionData.roll) / 500.0f;

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calculate g force on x axis with roll compensation."));
	this->motionData.rollCompensatedAccXG = this->motionData.accXG + sin(this->motionData.roll / 180.0f * PI);

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calculate g force on y axis with pitch compensation."));
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
uint8_t TesLight::MotionSensor::calibrate(const bool failOnTemperature)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calibrate the motion sensor."));
	if (failOnTemperature)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Checking motion sensor temperature."));
		TesLight::MPU6050::MPU6050MotionData sensorData;
		if (!this->mpu6050->getData(sensorData))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read sensor data from MPU6050."));
			return 1;
		}

		if (sensorData.temperatureDeg < 20.0f)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor becaues the temperature is too low."));
			return 2;
		}
		else if (sensorData.temperatureDeg > 40.0f)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Can not calibrate motion sensor becaues the temperature is too high."));
			return 3;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Collect data."));
	double calibrationData[12] = {0.0f};
	for (uint16_t i = 0; i < 100; i++)
	{
		TesLight::MPU6050::MPU6050MotionData sensorData;
		if (!this->mpu6050->getData(sensorData))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read sensor data from MPU6050."));
			return 1;
		}

		calibrationData[0] += sensorData.accXRaw / 100.0f;
		calibrationData[1] += sensorData.accYRaw / 100.0f;
		calibrationData[2] += sensorData.accZRaw / 100.0f;
		calibrationData[3] += sensorData.gyroXRaw / 100.0f;
		calibrationData[4] += sensorData.gyroYRaw / 100.0f;
		calibrationData[5] += sensorData.gyroZRaw / 100.0f;
		calibrationData[6] += sensorData.accXG / 100.0f;
		calibrationData[7] += sensorData.accYG / 100.0f;
		calibrationData[8] += sensorData.accZG / 100.0f;
		calibrationData[9] += sensorData.gyroXDeg / 100.0f;
		calibrationData[10] += sensorData.gyroYDeg / 100.0f;
		calibrationData[11] += sensorData.gyroZDeg / 100.0f;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Set data."));
	this->calibrationData.accXRaw = calibrationData[0];
	this->calibrationData.accYRaw = calibrationData[1];
	// this->calibrationData.accZRaw = calibrationData[2];
	this->calibrationData.gyroXRaw = calibrationData[3];
	this->calibrationData.gyroYRaw = calibrationData[4];
	this->calibrationData.gyroZRaw = calibrationData[5];
	this->calibrationData.accXG = calibrationData[6];
	this->calibrationData.accYG = calibrationData[7];
	// this->calibrationData.accZG = calibrationData[8];
	this->calibrationData.gyroXDeg = calibrationData[9];
	this->calibrationData.gyroYDeg = calibrationData[10];
	this->calibrationData.gyroZDeg = calibrationData[11];

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calibration successful."));
	return 0;
}

/**
 * @brief Set the calibration data of the motion sensor.
 * @param calibrationData full set of calibration data
 */
void TesLight::MotionSensor::setCalibration(TesLight::MotionSensor::CalibrationData calibrationData)
{
	this->calibrationData = calibrationData;
}

/**
 * @brief Get the current calibration data.
 * @return full set of calibration data
 */
TesLight::MotionSensor::CalibrationData TesLight::MotionSensor::getCalibration()
{
	return this->calibrationData;
}

/**
 * @brief Get the current motion data.
 * @return full set of motion data
 */
TesLight::MotionSensor::MotionSensorData TesLight::MotionSensor::getMotion()
{
	return this->motionData;
}
