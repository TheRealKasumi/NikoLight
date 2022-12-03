/**
 * @file MotionSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the motion sensor on the TesLight board.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <stdint.h>

#include "configuration/SystemConfiguration.h"
#include "configuration/Configuration.h"
#include "hardware/MPU6050.h"
#include "logging/Logger.h"

namespace TesLight
{
	class MotionSensor
	{
	public:
		enum MotionSensorValue
		{
			ACC_X_RAW,
			ACC_Y_RAW,
			ACC_Z_RAW,
			GY_X_RAW,
			GY_Y_RAW,
			GY_Z_RAW,
			ACC_X_G,
			ACC_Y_G,
			ACC_Z_G,
			GY_X_DEG,
			GY_Y_DEG,
			GY_Z_DEG,
			PITCH,
			ROLL,
			YAW,
			ROLL_COMPENSATED_ACC_X_G,
			PITCH_COMPENSATED_ACC_Y_G,
			TEMP_RAW,
			TEMP_DEG
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

		MotionSensor(const uint8_t sensorAddress, TesLight::Configuration *configuration);
		~MotionSensor();

		bool begin();
		bool run();
		uint8_t calibrate(const bool failOnTemperature);
		TesLight::MotionSensor::MotionSensorData getMotion();

	private:
		TesLight::MPU6050 *mpu6050;
		TesLight::Configuration *configuration;
		TesLight::MotionSensor::MotionSensorData motionData;
		unsigned long lastMeasure;
	};
}

#endif