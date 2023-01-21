/**
 * @file AnalogInput.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the analog input from the TesLight board.
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
#ifndef ANALOG_INPUT_H
#define ANALOG_INPUT_H

#include <Arduino.h>

namespace TL
{
	class AnalogInput
	{
	public:
		static void begin(const uint8_t inputPin);
		static void begin(const uint8_t inputPin, const uint8_t inputMode);
		static void begin(const uint8_t inputPin, const uint8_t inputMode, const float maxVoltage);
		static void end();
		static bool isInitialized();

		static void setInputPin(const uint8_t inputPin);
		static uint8_t getInputPin();

		static void setInputMode(const uint8_t inputMode);
		static uint8_t getInputMode();

		static void setMaxVoltage(const float maxVoltage);
		static float getMaxVoltage();

		static uint16_t getAnalogValue();
		static float getAnalogVoltage(const bool usePolynomialCorrection = true);

	private:
		AnalogInput();

		static bool initialized;
		static uint8_t inputPin;
		static uint8_t inputMode;
		static float maxVoltage;

		static void setupPin();
	};
}

#endif
