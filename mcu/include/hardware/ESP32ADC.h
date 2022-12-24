/**
 * @file ESP32ADC.h
 * @author TheRealKasumi
 * @brief Contains a class for reading analog values from any pin using the ESP32's ADC.
 *
 * @copyright Copyright (c) 2022
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