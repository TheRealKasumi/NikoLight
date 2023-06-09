/**
 * @file DS18B20.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the DS18B20 digital temperature sensor.
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
#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>
#include <vector>

#include "OneWire.h"

namespace NL
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

		enum class DS18B20Res : uint8_t
		{
			DS18B20_9_BIT = 0B00011111,
			DS18B20_10_BIT = 0B00111111,
			DS18B20_11_BIT = 0B01011111,
			DS18B20_12_BIT = 0B01111111
		};

		static NL::DS18B20::Error begin(const uint8_t busPin);
		static void end();
		static bool isInitialized();

		static size_t getNumSensors();
		static NL::DS18B20::Error getSensorAddress(uint64_t &sensorAddress, const size_t sensorIndex);

		static NL::DS18B20::Error setResolution(const NL::DS18B20::DS18B20Res resolution, const size_t sensorIndex);
		static NL::DS18B20::Error getResolution(NL::DS18B20::DS18B20Res &resolution, const size_t sensorIndex);

		static NL::DS18B20::Error startMeasurement(const size_t sensorIndex);
		static NL::DS18B20::Error isMeasurementReady(bool &isReady, const size_t sensorIndex);
		static NL::DS18B20::Error getTemperature(float &temp, const size_t sensorIndex);

	private:
		DS18B20();

		static bool initialized;
		static OneWire oneWire;
		static std::vector<uint64_t> sensorAddress;
		static std::vector<NL::DS18B20::DS18B20Res> resolution;
		static std::vector<float> lastMeasurement;
		static std::vector<unsigned long> measurementReadyTime;

		static NL::DS18B20::Error getSensors(std::vector<uint64_t> &sensorAddress);
	};
}

#endif