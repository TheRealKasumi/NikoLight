/**
 * @file ESP32ADC.h
 * @author TheRealKasumi
 * @brief Contains a class for reading analog values from any pin using the ESP32's ADC.
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
#ifndef ESP32ADC_H
#define ESP32ADC_H

#include <Arduino.h>
#include "logging/Logger.h"

namespace TL
{
	class ESP32ADC
	{
	public:
		ESP32ADC(const uint8_t inputPin);
		ESP32ADC(const uint8_t inputPin, const uint8_t inputMode);
		ESP32ADC(const uint8_t inputPin, const uint8_t inputMode, const float maxVoltage);
		~ESP32ADC();

		void setInputPin(const uint8_t inputPin);
		uint8_t getInputPin();

		void setInputMode(const uint8_t inputMode);
		uint8_t getInputMode();

		void setMaxVoltage(const float maxVoltage);
		float getMaxVoltage();

		uint16_t getAnalogValue();
		float getAnalogVoltage(const bool usePolynomialCorrection = true);

	private:
		uint8_t inputPin;
		uint8_t inputMode;
		float maxVoltage;

		void setupPin();
	};
}

#endif