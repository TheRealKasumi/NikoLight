/**
 * @file MotionSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the motion sensor on the NikoLight board.
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
#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <stdint.h>

#include "configuration/SystemConfiguration.h"
#include "configuration/Configuration.h"
#include "hardware/MPU6050.h"

namespace NL
{
	class MotionSensor
	{
	public:
		enum class Error
		{
			OK,						  // No error
			ERROR_CONFIG_UNAVAILABLE, // The configuration is not available
			ERROR_MPU6050_UNAVIALBLE, // The MPU6050 is not available
			ERROR_TOO_COLD,			  // The MPU6050 is too cold for a calibratin
			ERROR_TOO_WARM			  // The MPU6050 is too warm for a calibration
		};

		struct MotionSensorData
		{
			int16_t accXRaw;
			int16_t accYRaw;
			int16_t accZRaw;
			int16_t gyroXRaw;
			int16_t gyroYRaw;
			int16_t gyroZRaw;
			float accXG;
			float accYG;
			float accZG;
			float gyroXDeg;
			float gyroYDeg;
			float gyroZDeg;
			float pitch;
			float roll;
			float yaw;
			float rollCompensatedAccXG;
			float pitchCompensatedAccYG;
			int16_t temperatureRaw;
			float temperatureDeg;
		};

		static NL::MotionSensor::Error begin();
		static void end();
		static bool isInitialized();

		static NL::MotionSensor::Error run();
		static NL::MotionSensor::Error calibrate(const bool failOnTemperature);
		static NL::MotionSensor::MotionSensorData getMotion();

	private:
		MotionSensor();

		static bool initialized;
		static NL::MotionSensor::MotionSensorData motionData;
		static unsigned long lastMeasure;
	};
}

#endif