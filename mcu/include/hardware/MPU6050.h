/**
 * @file MPU6050.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the MPU6050 I²C motion sensor.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include <Wire.h>

#include "logging/Logger.h"

namespace TesLight
{
	class MPU6050
	{
	public:
		enum MPU6050AccScale
		{
			SCALE_2G = 0B00000000,
			SCALE_4G = 0B00001000,
			SCALE_8G = 0B00010000,
			SCALE_16G = 0B00011000
		};

		enum MPU6050GyScale
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

		MPU6050(const uint8_t deviceAddress);
		MPU6050(const uint8_t deviceAddress, const TesLight::MPU6050::MPU6050AccScale accScale, const TesLight::MPU6050::MPU6050GyScale gyScale);
		~MPU6050();

		bool begin();

		bool wake();
		bool sleep();

		bool setAccScale(TesLight::MPU6050::MPU6050AccScale accScale);
		TesLight::MPU6050::MPU6050AccScale getAccScale();

		bool setGyScale(TesLight::MPU6050::MPU6050GyScale gyScale);
		TesLight::MPU6050::MPU6050GyScale getGyScale();

		bool getData(TesLight::MPU6050::MPU6050MotionData &motionData);

	private:
		uint8_t deviceAddress;
		TesLight::MPU6050::MPU6050AccScale accScale;
		TesLight::MPU6050::MPU6050GyScale gyScale;

		float getScaleDiv(const TesLight::MPU6050::MPU6050AccScale accScale);
		float getScaleDiv(const TesLight::MPU6050::MPU6050GyScale gyScale);
	};
}

#endif