/**
 * @file MotionSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the motion sensor on the NikoLight board.
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