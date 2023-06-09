/**
 * @file motionSensor.cpp
 * @author TheRealKasumi
 * @brief implementation of the {@link NL::MotionSensor}.
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
#include "sensor/MotionSensor.h"

bool NL::MotionSensor::initialized = false;
NL::MotionSensor::MotionSensorData NL::MotionSensor::motionData;
unsigned long NL::MotionSensor::lastMeasure;

/**
 * @brief Initialize the motion sensor and set the scales.
 * @return OK when the motion sensor was initialized
 * @return ERROR_CONFIG_UNAVAILABLE when the configuration is not available
 * @return ERROR_MPU6050_UNAVIALBLE when the MPU6050 is not available
 */
NL::MotionSensor::Error NL::MotionSensor::begin()
{
	NL::MotionSensor::initialized = false;
	NL::MotionSensor::motionData.accXRaw = 0;
	NL::MotionSensor::motionData.accYRaw = 0;
	NL::MotionSensor::motionData.accZRaw = 0;
	NL::MotionSensor::motionData.gyroXRaw = 0;
	NL::MotionSensor::motionData.gyroYRaw = 0;
	NL::MotionSensor::motionData.gyroZRaw = 0;
	NL::MotionSensor::motionData.accXG = 0.0f;
	NL::MotionSensor::motionData.accYG = 0.0f;
	NL::MotionSensor::motionData.accZG = 0.0f;
	NL::MotionSensor::motionData.gyroXDeg = 0.0f;
	NL::MotionSensor::motionData.gyroYDeg = 0.0f;
	NL::MotionSensor::motionData.gyroZDeg = 0.0f;
	NL::MotionSensor::motionData.pitch = 0.0f;
	NL::MotionSensor::motionData.roll = 0.0f;
	NL::MotionSensor::motionData.yaw = 0.0f;
	NL::MotionSensor::motionData.rollCompensatedAccXG = 0.0f;
	NL::MotionSensor::motionData.pitchCompensatedAccYG = 0.0f;
	NL::MotionSensor::motionData.temperatureRaw = 0;
	NL::MotionSensor::motionData.temperatureDeg = 0;
	NL::MotionSensor::lastMeasure = 0;

	if (!NL::Configuration::isInitialized())
	{
		return NL::MotionSensor::Error::ERROR_CONFIG_UNAVAILABLE;
	}

	if (!NL::MPU6050::isInitialized())
	{
		return NL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
	}

	if (NL::MPU6050::setAccScale(NL::MPU6050::MPU6050AccScale::SCALE_4G) != NL::MPU6050::Error::OK)
	{
		return NL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
	}

	if (NL::MPU6050::setGyScale(NL::MPU6050::MPU6050GyScale::SCALE_500DS) != NL::MPU6050::Error::OK)
	{
		return NL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
	}

	NL::MotionSensor::initialized = true;
	return NL::MotionSensor::Error::OK;
}

/**
 * @brief Stop the motion sensor.
 */
void NL::MotionSensor::end()
{
	NL::MotionSensor::initialized = false;
}

/**
 * @brief Check if the motion sensor is initialized.
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::MotionSensor::isInitialized()
{
	return NL::MotionSensor::initialized;
}

/**
 * @brief Run the measurement and calculation cycle.
 * @return OK when the motion data was captured
 * @return ERROR_MPU6050_UNAVIALBLE when the MPU6050 is not available
 */
NL::MotionSensor::Error NL::MotionSensor::run()
{
	NL::MPU6050::MPU6050MotionData sensorData;
	if (NL::MPU6050::getData(sensorData) != NL::MPU6050::Error::OK)
	{
		return NL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
	}

	const NL::Configuration::MotionSensorCalibration calibrationData = NL::Configuration::getMotionSensorCalibration();
	const unsigned long timeStep = NL::MotionSensor::lastMeasure == 0 ? 0.0f : (micros() - NL::MotionSensor::lastMeasure);
	const float timeScale = timeStep / 1000000.0f;
	NL::MotionSensor::lastMeasure = micros();

	NL::MotionSensor::motionData.accXRaw = sensorData.accXRaw - calibrationData.accXRaw;
	NL::MotionSensor::motionData.accYRaw = sensorData.accYRaw - calibrationData.accYRaw;
	NL::MotionSensor::motionData.accZRaw = sensorData.accZRaw - calibrationData.accZRaw;
	NL::MotionSensor::motionData.gyroXRaw = sensorData.gyroXRaw - calibrationData.gyroXRaw;
	NL::MotionSensor::motionData.gyroYRaw = sensorData.gyroYRaw - calibrationData.gyroYRaw;
	NL::MotionSensor::motionData.gyroZRaw = sensorData.gyroZRaw - calibrationData.gyroZRaw;
	NL::MotionSensor::motionData.accXG = sensorData.accXG - calibrationData.accXG;
	NL::MotionSensor::motionData.accYG = sensorData.accYG - calibrationData.accYG;
	NL::MotionSensor::motionData.accZG = sensorData.accZG - calibrationData.accZG;
	NL::MotionSensor::motionData.gyroXDeg = sensorData.gyroXDeg - calibrationData.gyroXDeg;
	NL::MotionSensor::motionData.gyroYDeg = sensorData.gyroYDeg - calibrationData.gyroYDeg;
	NL::MotionSensor::motionData.gyroZDeg = sensorData.gyroZDeg - calibrationData.gyroZDeg;
	NL::MotionSensor::motionData.temperatureRaw = sensorData.temperatureRaw;
	NL::MotionSensor::motionData.temperatureDeg = sensorData.temperatureDeg;

	NL::MotionSensor::motionData.pitch += NL::MotionSensor::motionData.gyroXDeg * timeScale;
	NL::MotionSensor::motionData.roll += NL::MotionSensor::motionData.gyroYDeg * timeScale;
	NL::MotionSensor::motionData.yaw += NL::MotionSensor::motionData.gyroZDeg * timeScale;

	const float accPitch = atan(NL::MotionSensor::motionData.accYG / NL::MotionSensor::motionData.accZG) * 180.0f / PI;
	NL::MotionSensor::motionData.pitch += (accPitch - NL::MotionSensor::motionData.pitch) / 500.0f;

	const float accRoll = -atan(NL::MotionSensor::motionData.accXG / NL::MotionSensor::motionData.accZG) * 180.0f / PI;
	NL::MotionSensor::motionData.roll += (accRoll - NL::MotionSensor::motionData.roll) / 500.0f;

	NL::MotionSensor::motionData.rollCompensatedAccXG = NL::MotionSensor::motionData.accXG + sin(NL::MotionSensor::motionData.roll / 180.0f * PI);
	NL::MotionSensor::motionData.pitchCompensatedAccYG = NL::MotionSensor::motionData.accYG - sin(NL::MotionSensor::motionData.pitch / 180.0f * PI);

	return NL::MotionSensor::Error::OK;
}

/**
 * @brief Calibrate the motion sensor.
 * @param failOnTemperature if set to true, the calibration will fail when the sensor is too cold or warm
 * @return OK when the sensor was calibrated
 * @return ERROR_MPU6050_UNAVIALBLE when the MPU6050 is not available
 * @return ERROR_TOO_COLD when the MPU6050 is too cold
 * @return ERROR_TOO_WARM when the MPU6050 is too warm
 */
NL::MotionSensor::Error NL::MotionSensor::calibrate(const bool failOnTemperature)
{
	if (failOnTemperature)
	{
		NL::MPU6050::MPU6050MotionData sensorData;
		const NL::MPU6050::Error mpuError = NL::MPU6050::getData(sensorData);
		if (mpuError != NL::MPU6050::Error::OK)
		{
			return NL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
		}

		if (sensorData.temperatureDeg < 15.0f)
		{
			return NL::MotionSensor::Error::ERROR_TOO_COLD;
		}
		else if (sensorData.temperatureDeg > 40.0f)
		{
			return NL::MotionSensor::Error::ERROR_TOO_WARM;
		}
	}

	double calibrationData[12] = {0.0f};
	for (uint16_t i = 0; i < 1000; i++)
	{
		NL::MPU6050::MPU6050MotionData sensorData;
		const NL::MPU6050::Error mpuError = NL::MPU6050::getData(sensorData);
		if (mpuError != NL::MPU6050::Error::OK)
		{
			return NL::MotionSensor::Error::ERROR_MPU6050_UNAVIALBLE;
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

	NL::Configuration::MotionSensorCalibration calibration = NL::Configuration::getMotionSensorCalibration();
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
	NL::Configuration::setMotionSensorCalibration(calibration);

	return NL::MotionSensor::Error::OK;
}

/**
 * @brief Get the current motion data.
 * @return full set of motion data
 */
NL::MotionSensor::MotionSensorData NL::MotionSensor::getMotion()
{
	return NL::MotionSensor::motionData;
}
