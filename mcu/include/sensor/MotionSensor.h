/**
 * @file MotionSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading motion data from the MPU6050.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <stdint.h>
#include <Wire.h>

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
		};

		typedef struct MotionSensorData
		{
			int16_t accXRaw;
			int16_t accYRaw;
			int16_t accZRaw;
			int16_t gyroXRaw;
			int16_t gyroYRaw;
			int16_t gyroZRaw;
			double accXG;
			double accYG;
			double accZG;
			double gyroXDeg;
			double gyroYDeg;
			double gyroZDeg;
		} MotionSensorData;

		MotionSensor(const uint8_t deviceAddress, const uint8_t sdaPin = -1, const uint8_t sclPin = -1, const uint8_t bufferSize = 5);
		~MotionSensor();

		bool begin(const bool autoCalibrate = true);

		bool readData(const bool asRingBuffer = true);
		TesLight::MotionSensor::MotionSensorData getData();

		void setOffsetData(TesLight::MotionSensor::MotionSensorData offsetData);
		TesLight::MotionSensor::MotionSensorData getOffsetData();

	private:
		uint8_t deviceAddress;
		uint8_t sdaPin;
		uint8_t sclPin;
		uint8_t bufferSize;
		uint8_t bufferIndex;
		TesLight::MotionSensor::MotionSensorData *motionData;
		TesLight::MotionSensor::MotionSensorData offsetData;

		void initializeMotionSensorData(TesLight::MotionSensor::MotionSensorData *data, const uint8_t sampleSize);
		bool calculateOffset(const uint8_t sampleSize, const uint32_t sampleDelay);
		TesLight::MotionSensor::MotionSensorData average(TesLight::MotionSensor::MotionSensorData *data, const uint8_t sampleSize);
	};
}

#endif