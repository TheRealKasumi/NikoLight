/**
 * @file MotionSensor.h
 * @author TheRealKasumi
 * @brief Contains a class for reading motion data from the MPU6050.
 * @version 0.0.1
 * @date 2022-06-22
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

	class MotionSensor
	{
	public:
		MotionSensor(const uint8_t deviceAddress, const uint8_t sdaPin = -1, const uint8_t sclPin = -1, const uint8_t bufferSize = 5);
		~MotionSensor();

		bool begin(const bool autoCalibrate = true);

		bool readData(const bool asRingBuffer = true);
		TesLight::MotionSensorData getData();

		void setOffsetData(TesLight::MotionSensorData offsetData);
		TesLight::MotionSensorData getOffsetData();

	private:
		uint8_t deviceAddress;
		uint8_t sdaPin;
		uint8_t sclPin;
		uint8_t bufferSize;
		uint8_t bufferIndex;
		TesLight::MotionSensorData *motionData;
		TesLight::MotionSensorData offsetData;

		void initializeMotionSensorData(TesLight::MotionSensorData *data, const uint8_t sampleSize);
		bool calculateOffset(const uint8_t sampleSize, const uint32_t sampleDelay);
		TesLight::MotionSensorData average(TesLight::MotionSensorData *data, const uint8_t sampleSize);
	};
}

#endif