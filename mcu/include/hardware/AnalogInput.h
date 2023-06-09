/**
 * @file AnalogInput.h
 * @author TheRealKasumi
 * @brief Contains a class for reading the analog input from the NikoLight board.
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
#ifndef ANALOG_INPUT_H
#define ANALOG_INPUT_H

#include <Arduino.h>

namespace NL
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
