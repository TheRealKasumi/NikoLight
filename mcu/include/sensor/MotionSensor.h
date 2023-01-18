/**
 * @file MotionSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the motion sensor on the TesLight board.
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

namespace TL
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

		enum MotionSensorValue : uint8_t
		{
			ACC_X_RAW = 0,					// Raw x acceleration
			ACC_Y_RAW = 1,					// Raw y acceleration
			ACC_Z_RAW = 2,					// Raw z acceleration
			GY_X_RAW = 3,					// Raw x rotation
			GY_Y_RAW = 4,					// Raw y rotation
			GY_Z_RAW = 5,					// Raw z rotation
			ACC_X_G = 6,					// Acceleration on x axis in g
			ACC_Y_G = 7,					// Acceleration on y axis in g
			ACC_Z_G = 8,					// Acceleration on z axis in g
			GY_X_DEG = 9,					// Rotation around the x axis in deg/s
			GY_Y_DEG = 10,					// Rotation around the y axis in deg/s
			GY_Z_DEG = 11,					// Rotation around the z axis in deg/s
			PITCH = 12,						// Pitch angle of the unit
			ROLL = 13,						// Roll angle of the unit
			YAW = 14,						// Yaw angle of the unit
			ROLL_COMPENSATED_ACC_X_G = 15,	// Roll angle compensated acceleration on x axis in g
			PITCH_COMPENSATED_ACC_Y_G = 16, // Pitch angle compensated acceleration on y axis in g
			TEMP_RAW = 17,					// Raw temperature value
			TEMP_DEG = 18					// Temperature in degree celsius
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

		static TL::MotionSensor::Error begin();
		static void end();
		static bool isInitialized();

		static TL::MotionSensor::Error run();
		static TL::MotionSensor::Error calibrate(const bool failOnTemperature);
		static TL::MotionSensor::MotionSensorData getMotion();

	private:
		MotionSensor();

		static bool initialized;
		static TL::MotionSensor::MotionSensorData motionData;
		static unsigned long lastMeasure;
	};
}

#endif