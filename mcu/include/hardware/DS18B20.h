/**
 * @file DS18B20.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the DS18B20 digital temperature sensor.
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
#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>
#include <memory>
#include <vector>

#include "OneWire.h"
#include "logging/Logger.h"

namespace TL
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

		size_t getNumSensors();
		uint64_t getSensorAddress(const size_t sensorIndex);

		bool setResolution(const size_t sensorIndex, const TL::DS18B20::DS18B20Res resolution);
		bool getResolution(const size_t sensorIndex, TL::DS18B20::DS18B20Res &resolution);

		bool startMeasurement(const size_t sensorIndex);
		bool isMeasurementReady(const size_t sensorIndex);
		bool getTemperature(const size_t sensorIndex, float &temp);

	private:
		std::unique_ptr<OneWire> oneWire;
		std::vector<uint64_t> sensorAddress;
		std::vector<TL::DS18B20::DS18B20Res> resolution;
		std::vector<float> lastMeasurement;
		std::vector<unsigned long> measurementReadyTime;

		bool getSensors(std::vector<uint64_t> &sensorAddress);
	};
}

#endif