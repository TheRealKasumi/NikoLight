/**
 * @file BH1750.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the BH1750 I²C light sensor.
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
#ifndef BH1750_H
#define BH1750_H

#include <stdint.h>
#include <Wire.h>

namespace NL
{
	class BH1750
	{
	public:
		enum class Error
		{
			OK,			   // No error
			ERROR_IIC_COMM // I²C communication failed
		};

		enum class BH1750Res : uint8_t
		{
			BH1750_HIGH = 0b00010000,
			BH1750_LOW = 0b00010011
		};

		static NL::BH1750::Error begin(const uint8_t deviceAddress);
		static NL::BH1750::Error begin(const uint8_t deviceAddress, const NL::BH1750::BH1750Res resolution);
		static void end();
		static bool isInitialized();

		static NL::BH1750::Error setResolution(const NL::BH1750::BH1750Res resolution);
		static NL::BH1750::BH1750Res getResolution();

		static NL::BH1750::Error getLux(float &lux);

	private:
		BH1750();

		static bool initialized;
		static uint8_t deviceAddress;
		static NL::BH1750::BH1750Res resolution;

		static NL::BH1750::Error write(const uint8_t command);
		static NL::BH1750::Error read(uint16_t &value);
	};
}

#endif