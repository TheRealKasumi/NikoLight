/**
 * @file DS18B20.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the DS18B20 digital temperature sensor.
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
#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>
#include <vector>

#include "OneWire.h"

namespace TL
{
	class DS18B20
	{
	public:
		enum class Error
		{
			OK,					// No error
			ERROR_OW_COMM,		// OneWire bus communication error
			ERROR_OW_CRC,		// Invalid crc received
			ERROR_OUT_OF_BOUNDS // Index is out of bounds
		};

		enum DS18B20Res : uint8_t
		{
			DS18B20_9_BIT = 0B00011111,
			DS18B20_10_BIT = 0B00111111,
			DS18B20_11_BIT = 0B01011111,
			DS18B20_12_BIT = 0B01111111
		};

		static TL::DS18B20::Error begin(const uint8_t busPin);
		static void end();
		static bool isInitialized();

		static size_t getNumSensors();
		static TL::DS18B20::Error getSensorAddress(uint64_t &sensorAddress, const size_t sensorIndex);

		static TL::DS18B20::Error setResolution(const TL::DS18B20::DS18B20Res resolution, const size_t sensorIndex);
		static TL::DS18B20::Error getResolution(TL::DS18B20::DS18B20Res &resolution, const size_t sensorIndex);

		static TL::DS18B20::Error startMeasurement(const size_t sensorIndex);
		static TL::DS18B20::Error isMeasurementReady(bool &isReady, const size_t sensorIndex);
		static TL::DS18B20::Error getTemperature(float &temp, const size_t sensorIndex);

	private:
		DS18B20();

		static bool initialized;
		static OneWire oneWire;
		static std::vector<uint64_t> sensorAddress;
		static std::vector<TL::DS18B20::DS18B20Res> resolution;
		static std::vector<float> lastMeasurement;
		static std::vector<unsigned long> measurementReadyTime;

		static TL::DS18B20::Error getSensors(std::vector<uint64_t> &sensorAddress);
	};
}

#endif