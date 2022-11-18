/**
 * @file DS18B20.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the DS18B20 digital temperature sensor.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>
#include "OneWire.h"
#include "logging/Logger.h"

namespace TesLight
{
	class DS18B20
	{
	public:
		enum DS18B20Res
		{
			DS18B20_9_BIT = 0B00011111,
			DS18B20_10_BIT = 0B00111111,
			DS18B20_11_BIT = 0B01011111,
			DS18B20_12_BIT = 0B01111111
		};

		DS18B20(const uint8_t busPin);
		~DS18B20();

		bool begin();

		uint8_t getNumSensors();
		uint64_t getSensorAddress(const uint8_t sensorIndex);

		bool setResolution(const uint8_t sensorIndex, const TesLight::DS18B20::DS18B20Res resolution);
		bool getResolution(const uint8_t sensorIndex, TesLight::DS18B20::DS18B20Res &resolution);

		bool startMeasurement(const uint8_t sensorIndex);
		bool isMeasurementReady(const uint8_t sensorIndex);
		bool getTemperature(const uint8_t sensorIndex, float &temp);

	private:
		OneWire *oneWire;
		uint8_t numSensors;
		uint64_t *sensorAddress;
		TesLight::DS18B20::DS18B20Res *resolution;
		float *lastMeasurement;
		unsigned long *measurementReadyTime;

		bool getSensors(uint8_t &sensorCount, uint64_t sensorAddresses[], const uint8_t bufferSize);
	};
}

#endif