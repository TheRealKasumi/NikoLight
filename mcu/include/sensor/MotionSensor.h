/**
 * @file MotionSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the motion sensor on the TesLight board.
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
#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <stdint.h>
#include <memory>

#include "configuration/SystemConfiguration.h"
#include "configuration/Configuration.h"
#include "hardware/MPU6050.h"
#include "logging/Logger.h"

namespace TL
{
	class MotionSensor
	{
	public:
		enum MotionSensorValue
		{
			ACC_X_RAW = 0,
			ACC_Y_RAW = 1,
			ACC_Z_RAW = 2,
			GY_X_RAW = 3,
			GY_Y_RAW = 4,
			GY_Z_RAW = 5,
			ACC_X_G = 6,
			ACC_Y_G = 7,
			ACC_Z_G = 8,
			GY_X_DEG = 9,
			GY_Y_DEG = 10,
			GY_Z_DEG = 11,
			PITCH = 12,
			ROLL = 13,
			YAW = 14,
			ROLL_COMPENSATED_ACC_X_G = 15,
			PITCH_COMPENSATED_ACC_Y_G = 16,
			TEMP_RAW = 17,
			TEMP_DEG = 18
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

		MotionSensor(const uint8_t sensorAddress, TL::Configuration *configuration);
		~MotionSensor();

		bool begin();
		bool run();
		uint8_t calibrate(const bool failOnTemperature);
		TL::MotionSensor::MotionSensorData getMotion();

	private:
		std::unique_ptr<TL::MPU6050> mpu6050;
		TL::Configuration *configuration;
		TL::MotionSensor::MotionSensorData motionData;
		unsigned long lastMeasure;
	};
}

#endif