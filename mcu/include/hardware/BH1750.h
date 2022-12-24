/**
 * @file BH1750.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the BH1750 I²C light sensor.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef BH1750_H
#define BH1750_H

#include <stdint.h>
#include <Wire.h>
#include "logging/Logger.h"

namespace TL
{
	class BH1750
	{
	public:
		enum BH1750Res
		{
			BH1750_HIGH = 0b00010000,
			BH1750_LOW = 0b00010011
		};

		BH1750(const uint8_t deviceAddress);
		BH1750(const uint8_t deviceAddress, const TL::BH1750::BH1750Res resolution);
		~BH1750();

		bool begin();

		bool setResolution(const TL::BH1750::BH1750Res resolution);
		TL::BH1750::BH1750Res getResolution();

		bool getLux(float &lux);

	private:
		uint8_t deviceAddress;
		TL::BH1750::BH1750Res resolution;

		bool write(const uint8_t command);
		bool read(uint16_t &value);
	};
}

#endif