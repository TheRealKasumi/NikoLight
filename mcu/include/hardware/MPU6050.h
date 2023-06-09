/**
 * @file MPU6050.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the MPU6050 I²C motion sensor.
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
#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include <Wire.h>

namespace NL
{
	class MPU6050
	{
	public:
		enum class Error
		{
			OK,			   // No error
			ERROR_IIC_COMM // Error in the I²C communication
		};

		enum class MPU6050AccScale : uint8_t
		{
			SCALE_2G = 0B00000000,
			SCALE_4G = 0B00001000,
			SCALE_8G = 0B00010000,
			SCALE_16G = 0B00011000
		};

		enum class MPU6050GyScale : uint8_t
		{
			SCALE_250DS = 0B00000000,
			SCALE_500DS = 0B00001000,
			SCALE_1000DS = 0B00010000,
			SCALE_2000DS = 0B00011000
		};

		struct MPU6050MotionData
		{
			int16_t accXRaw;
			int16_t accYRaw;
			int16_t accZRaw;
			int16_t gyroXRaw;
			int16_t gyroYRaw;
			int16_t gyroZRaw;
			int16_t temperatureRaw;
			float accXG;
			float accYG;
			float accZG;
			float gyroXDeg;
			float gyroYDeg;
			float gyroZDeg;
			float temperatureDeg;
		};

		static NL::MPU6050::Error begin(const uint8_t deviceAddress);
		static NL::MPU6050::Error begin(const uint8_t deviceAddress, const NL::MPU6050::MPU6050AccScale accScale, const NL::MPU6050::MPU6050GyScale gyScale);
		static void end();
		static bool isInitialized();

		static NL::MPU6050::Error wake();
		static NL::MPU6050::Error sleep();

		static NL::MPU6050::Error setAccScale(NL::MPU6050::MPU6050AccScale accScale);
		static NL::MPU6050::MPU6050AccScale getAccScale();

		static NL::MPU6050::Error setGyScale(NL::MPU6050::MPU6050GyScale gyScale);
		static NL::MPU6050::MPU6050GyScale getGyScale();

		static NL::MPU6050::Error getData(NL::MPU6050::MPU6050MotionData &motionData);

	private:
		MPU6050();

		static bool initialized;
		static uint8_t deviceAddress;
		static NL::MPU6050::MPU6050AccScale accScale;
		static NL::MPU6050::MPU6050GyScale gyScale;

		static float getScaleDiv(const NL::MPU6050::MPU6050AccScale accScale);
		static float getScaleDiv(const NL::MPU6050::MPU6050GyScale gyScale);
	};
}

#endif