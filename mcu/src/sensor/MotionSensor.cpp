/**
 * @file motionSensor.cpp
 * @author TheRealKasumi
 * @brief implementation of the {@link TL::MotionSensor}.
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
#include "sensor/MotionSensor.h"

bool TL::MotionSensor::initialized = false;
TL::MotionSensor::MotionSensorData TL::MotionSensor::motionData;
unsigned long TL::MotionSensor::lastMeasure;

/**
 * @brief Initialize the motion sensor and set the scales.
 * @return OK when the motion sensor was initialized
 * @return ERROR_CONFIG_UNAVAILABLE when the configuration is not available
 * @return ERROR_MPU6050_UNAVIALBLE when the MPU6050 is not available
 */
TL::MotionSensor::Error TL::MotionSensor::begin()
{
	TL::MotionSensor::initialized = false;
	TL::MotionSensor::motionData.accXRaw = 0;
	TL::MotionSensor::motionData.accYRaw = 0;
	TL::MotionSensor::motionData.accZRaw = 0;
	TL::MotionSensor::motionData.gyroXRaw = 0;
	TL::MotionSensor::motionData.gyroYRaw = 0;
	TL::MotionSensor::motionData.gyroZRaw = 0;
	TL::MotionSensor::motionData.accXG = 0.0f;
	TL::MotionSensor::motionData.accYG = 0.0f;
	TL::MotionSensor::motionData.accZG = 0.0f;
	TL::MotionSensor::motionData.gyroXDeg = 0.0f;
	TL::MotionSensor::motionData.gyroYDeg = 0.0f;
	TL::MotionSensor::motionData.gyroZDeg = 0.0f;
	TL::MotionSensor::motionData.pitch = 0.0f;
	TL::MotionSensor::motionData.roll = 0.0f;
	TL::MotionSensor::motionData.yaw = 0.0f;
	TL::MotionSensor::motionData.rollCompensatedAccXG = 0.0f;
	TL::MotionSensor::motionData.pitchCompensatedAccYG = 0.0f;
	TL::MotionSensor::motionData.temperatureRaw = 0;
	TL::MotionSensor::motionData.temperatureDeg = 0;
	TL::MotionSensor::lastMeasure = 0;

	if (!TL::Configuration::isInitialized())
	{
		return TL::MotionSensor::Error::ERROR_CONFIG_UNAVAILABLE;
	}

	if (!TL::MPU6050::isInitialized())
	{
		return TL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
	}

	if (TL::MPU6050::setAccScale(TL::MPU6050::MPU6050AccScale::SCALE_4G) != TL::MPU6050::Error::OK)
	{
		return TL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
	}

	if (TL::MPU6050::setGyScale(TL::MPU6050::MPU6050GyScale::SCALE_500DS) != TL::MPU6050::Error::OK)
	{
		return TL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
	}

	TL::MotionSensor::initialized = true;
	return TL::MotionSensor::Error::OK;
}

/**
 * @brief Stop the motion sensor.
 */
void TL::MotionSensor::end()
{
	TL::MotionSensor::initialized = false;
}

/**
 * @brief Check if the motion sensor is initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool TL::MotionSensor::isInitialized()
{
	return TL::MotionSensor::initialized;
}

/**
 * @brief Run the measurement and calculation cycle.
 * @return OK when the motion data was captured
 * @return ERROR_MPU6050_UNAVIALBLE when the MPU6050 is not available
 */
TL::MotionSensor::Error TL::MotionSensor::run()
{
	TL::MPU6050::MPU6050MotionData sensorData;
	if (TL::MPU6050::getData(sensorData) != TL::MPU6050::Error::OK)
	{
		return TL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
	}

	const TL::Configuration::MotionSensorCalibration calibrationData = TL::Configuration::getMotionSensorCalibration();
	const unsigned long timeStep = TL::MotionSensor::lastMeasure == 0 ? 0.0f : (micros() - TL::MotionSensor::lastMeasure);
	const float timeScale = timeStep / 1000000.0f;
	TL::MotionSensor::lastMeasure = micros();

	TL::MotionSensor::motionData.accXRaw = sensorData.accXRaw - calibrationData.accXRaw;
	TL::MotionSensor::motionData.accYRaw = sensorData.accYRaw - calibrationData.accYRaw;
	TL::MotionSensor::motionData.accZRaw = sensorData.accZRaw - calibrationData.accZRaw;
	TL::MotionSensor::motionData.gyroXRaw = sensorData.gyroXRaw - calibrationData.gyroXRaw;
	TL::MotionSensor::motionData.gyroYRaw = sensorData.gyroYRaw - calibrationData.gyroYRaw;
	TL::MotionSensor::motionData.gyroZRaw = sensorData.gyroZRaw - calibrationData.gyroZRaw;
	TL::MotionSensor::motionData.accXG = sensorData.accXG - calibrationData.accXG;
	TL::MotionSensor::motionData.accYG = sensorData.accYG - calibrationData.accYG;
	TL::MotionSensor::motionData.accZG = sensorData.accZG - calibrationData.accZG;
	TL::MotionSensor::motionData.gyroXDeg = sensorData.gyroXDeg - calibrationData.gyroXDeg;
	TL::MotionSensor::motionData.gyroYDeg = sensorData.gyroYDeg - calibrationData.gyroYDeg;
	TL::MotionSensor::motionData.gyroZDeg = sensorData.gyroZDeg - calibrationData.gyroZDeg;
	TL::MotionSensor::motionData.temperatureRaw = sensorData.temperatureRaw;
	TL::MotionSensor::motionData.temperatureDeg = sensorData.temperatureDeg;

	TL::MotionSensor::motionData.pitch += TL::MotionSensor::motionData.gyroXDeg * timeScale;
	TL::MotionSensor::motionData.roll += TL::MotionSensor::motionData.gyroYDeg * timeScale;
	TL::MotionSensor::motionData.yaw += TL::MotionSensor::motionData.gyroZDeg * timeScale;

	const float accPitch = atan(TL::MotionSensor::motionData.accYG / TL::MotionSensor::motionData.accZG) * 180.0f / PI;
	TL::MotionSensor::motionData.pitch += (accPitch - TL::MotionSensor::motionData.pitch) / 500.0f;

	const float accRoll = -atan(TL::MotionSensor::motionData.accXG / TL::MotionSensor::motionData.accZG) * 180.0f / PI;
	TL::MotionSensor::motionData.roll += (accRoll - TL::MotionSensor::motionData.roll) / 500.0f;

	TL::MotionSensor::motionData.rollCompensatedAccXG = TL::MotionSensor::motionData.accXG + sin(TL::MotionSensor::motionData.roll / 180.0f * PI);
	TL::MotionSensor::motionData.pitchCompensatedAccYG = TL::MotionSensor::motionData.accYG - sin(TL::MotionSensor::motionData.pitch / 180.0f * PI);

	return TL::MotionSensor::Error::OK;
}

/**
 * @brief Calibrate the motion sensor.
 * @param failOnTemperature if set to true, the calibration will fail when the sensor is too cold or warm
 * @return OK when the sensor was calibrated
 * @return ERROR_MPU6050_UNAVIALBLE when the MPU6050 is not available
 * @return ERROR_TOO_COLD when the MPU6050 is too cold
 * @return ERROR_TOO_WARM when the MPU6050 is too warm
 */
TL::MotionSensor::Error TL::MotionSensor::calibrate(const bool failOnTemperature)
{
	if (failOnTemperature)
	{
		TL::MPU6050::MPU6050MotionData sensorData;
		const TL::MPU6050::Error mpuError = TL::MPU6050::getData(sensorData);
		if (mpuError != TL::MPU6050::Error::OK)
		{
			return TL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
		}

		if (sensorData.temperatureDeg < 15.0f)
		{
			return TL::MotionSensor::Error::ERROR_TOO_COLD;
		}
		else if (sensorData.temperatureDeg > 40.0f)
		{
			return TL::MotionSensor::Error::ERROR_TOO_WARM;
		}
	}

	double calibrationData[12] = {0.0f};
	for (uint16_t i = 0; i < 1000; i++)
	{
		TL::MPU6050::MPU6050MotionData sensorData;
		const TL::MPU6050::Error mpuError = TL::MPU6050::getData(sensorData);
		if (mpuError != TL::MPU6050::Error::OK)
		{
			return TL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
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

	TL::Configuration::MotionSensorCalibration calibration = TL::Configuration::getMotionSensorCalibration();
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
	TL::Configuration::setMotionSensorCalibration(calibration);

	return TL::MotionSensor::Error::OK;
}

/**
 * @brief Get the current motion data.
 * @return full set of motion data
 */
TL::MotionSensor::MotionSensorData TL::MotionSensor::getMotion()
{
	return TL::MotionSensor::motionData;
}
