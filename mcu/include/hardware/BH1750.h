/**
 * @file BH1750.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the BH1750 I²C light sensor.
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